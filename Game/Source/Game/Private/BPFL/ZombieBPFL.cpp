#include "BPFL/ZombieBPFL.h"

#if WITH_EDITOR

#include "Engine/StaticMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Scene.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/DirectionalLightComponent.h"

#include "PreviewScene.h"
#include "ThumbnailRendering/SceneThumbnailInfo.h"

#include "ImageUtils.h"
#include "ImageCore.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "HAL/FileManager.h"

#include "RenderingThread.h"
#include "UnrealClient.h"

namespace StaticMeshThumbnailExporter
{
	constexpr float ThumbnailFOV = 30.0f;

	// Render 3x larger and downsample.
	// Gives clean transparent edges without relying on SceneCapture AA.
	constexpr int32 SuperSample = 3;

	// Lighting
	constexpr float KeyLightIntensity = 3.0f;
	constexpr float FillLightIntensity = 2.0f;
	constexpr float BackLightIntensity = 1.0f;

	constexpr float SkyBrightness = 2.0f;

	// ============================================================
	// Render targets
	// ============================================================

	UTextureRenderTarget2D* CreateColorRenderTarget(
		const int32 Size)
	{
		UTextureRenderTarget2D* RenderTarget =
			NewObject<UTextureRenderTarget2D>(
				GetTransientPackage(),
				NAME_None,
				RF_Transient
			);

		if (!RenderTarget)
		{
			return nullptr;
		}

		/*
		 * IMPORTANT:
		 *
		 * FinalColorLDR is already display color.
		 *
		 * We want an sRGB 8-bit render target.
		 * Do NOT use RGBA16F here and do not manually
		 * convert the result to/from gamma multiple times.
		 */
		RenderTarget->RenderTargetFormat =
			ETextureRenderTargetFormat::RTF_RGBA8_SRGB;

		RenderTarget->ClearColor =
			FLinearColor::Black;

		RenderTarget->bForceLinearGamma = false;
		RenderTarget->bAutoGenerateMips = false;

		RenderTarget->InitAutoFormat(
			Size,
			Size
		);

		RenderTarget->UpdateResourceImmediate(true);

		return RenderTarget;
	}

	UTextureRenderTarget2D* CreateDepthRenderTarget(
		const int32 Size)
	{
		UTextureRenderTarget2D* RenderTarget =
			NewObject<UTextureRenderTarget2D>(
				GetTransientPackage(),
				NAME_None,
				RF_Transient
			);

		if (!RenderTarget)
		{
			return nullptr;
		}

		RenderTarget->RenderTargetFormat =
			ETextureRenderTargetFormat::RTF_R32f;

		/*
		 * SceneDepth background behavior can vary,
		 * therefore we don't actually rely on this exact
		 * value later. We determine background depth
		 * from the corners of the image.
		 */
		RenderTarget->ClearColor =
			FLinearColor(
				1000000.0f,
				0.0f,
				0.0f,
				0.0f
			);

		RenderTarget->bForceLinearGamma = true;
		RenderTarget->bAutoGenerateMips = false;

		RenderTarget->InitAutoFormat(
			Size,
			Size
		);

		RenderTarget->UpdateResourceImmediate(true);

		return RenderTarget;
	}

	// ============================================================
	// Read render targets
	// ============================================================

	bool ReadColorPixels(
		UTextureRenderTarget2D* RenderTarget,
		TArray<FColor>& OutPixels)
	{
		if (!IsValid(RenderTarget))
		{
			return false;
		}

		FTextureRenderTargetResource* Resource =
			RenderTarget->GameThread_GetRenderTargetResource();

		if (!Resource)
		{
			return false;
		}

		FReadSurfaceDataFlags Flags(RCM_UNorm);

		/*
		 * Do NOT perform another gamma conversion here.
		 *
		 * Render target is already sRGB.
		 */
		Flags.SetLinearToGamma(false);

		return Resource->ReadPixels(
			OutPixels,
			Flags
		);
	}

	bool ReadDepthPixels(
		UTextureRenderTarget2D* RenderTarget,
		TArray<FLinearColor>& OutPixels)
	{
		if (!IsValid(RenderTarget))
		{
			return false;
		}

		FTextureRenderTargetResource* Resource =
			RenderTarget->GameThread_GetRenderTargetResource();

		if (!Resource)
		{
			return false;
		}

		/*
		 * MinMax is important for float depth.
		 * We don't want depth normalized to 0..1.
		 */
		FReadSurfaceDataFlags Flags(RCM_MinMax);

		Flags.SetLinearToGamma(false);

		return Resource->ReadLinearColorPixels(
			OutPixels,
			Flags
		);
	}

	// ============================================================
	// Camera
	// ============================================================

	void GetThumbnailCamera(
		UStaticMesh* StaticMesh,
		FVector& OutLocation,
		FRotator& OutRotation)
	{
		const FBoxSphereBounds Bounds =
			StaticMesh->GetBounds();

		float OrbitPitch = -15.0f;
		float OrbitYaw = -45.0f;
		float OrbitZoom = 0.0f;

		/*
		 * Use thumbnail rotation saved on the Static Mesh.
		 */
		if (const USceneThumbnailInfo* ThumbnailInfo =
			Cast<USceneThumbnailInfo>(
				StaticMesh->GetThumbnailInfo()))
		{
			OrbitPitch =
				ThumbnailInfo->OrbitPitch;

			OrbitYaw =
				ThumbnailInfo->OrbitYaw;

			OrbitZoom =
				ThumbnailInfo->OrbitZoom;
		}

		const float Radius =
			FMath::Max(
				Bounds.SphereRadius,
				1.0f
			);

		const float HalfFOV =
			FMath::DegreesToRadians(
				ThumbnailFOV * 0.5f
			);

		float Distance =
			Radius /
			FMath::Tan(HalfFOV);

		// Some free space around the object.
		Distance *= 1.15f;

		Distance += OrbitZoom;

		Distance =
			FMath::Max(
				Distance,
				Radius * 1.2f
			);

		const FRotator OrbitRotation(
			OrbitPitch,
			OrbitYaw,
			0.0f
		);

		const FVector Direction =
			OrbitRotation.Vector();

		OutLocation =
			-Direction * Distance;

		OutRotation =
			(-OutLocation).Rotation();
	}

	// ============================================================
	// Additional light
	// ============================================================

	UDirectionalLightComponent* AddFillLight(
		FPreviewScene& PreviewScene,
		const FRotator& Rotation,
		const float Intensity)
	{
		UDirectionalLightComponent* Light =
			NewObject<UDirectionalLightComponent>(
				GetTransientPackage(),
				NAME_None,
				RF_Transient
			);

		if (!Light)
		{
			return nullptr;
		}

		Light->SetIntensity(Intensity);

		Light->SetLightColor(
			FLinearColor::White
		);

		/*
		 * Fill lights must not create additional
		 * ugly shadows on a small icon.
		 */
		Light->SetCastShadows(false);

		PreviewScene.AddComponent(
			Light,
			FTransform(Rotation)
		);

		return Light;
	}

	// ============================================================
	// Calculate background depth
	// ============================================================

	float GetBackgroundDepth(
		const TArray<FLinearColor>& DepthPixels,
		const int32 Size)
	{
		/*
		 * The mesh never reaches the corners because the camera
		 * has padding around it.
		 *
		 * Use all four corners to determine whatever value
		 * SceneDepth uses for the background on this renderer.
		 */

		const float TopLeft =
			DepthPixels[0].R;

		const float TopRight =
			DepthPixels[Size - 1].R;

		const float BottomLeft =
			DepthPixels[(Size - 1) * Size].R;

		const float BottomRight =
			DepthPixels[Size * Size - 1].R;

		return (
			TopLeft +
			TopRight +
			BottomLeft +
			BottomRight
			) * 0.25f;
	}

	bool IsMeshPixel(
		const float Depth,
		const float BackgroundDepth)
	{
		if (!FMath::IsFinite(Depth))
		{
			return false;
		}

		/*
		 * Works whether the renderer gives us:
		 *
		 * background = 0
		 *
		 * or
		 *
		 * background = a very large depth.
		 */

		const float Tolerance =
			FMath::Max(
				0.001f,
				FMath::Abs(BackgroundDepth) *
				0.00001f
			);

		return FMath::Abs(
			Depth - BackgroundDepth
		) > Tolerance;
	}

	// ============================================================
	// Downsample
	// ============================================================

	void Downsample(
		const TArray<FColor>& SourceColor,
		const TArray<FLinearColor>& SourceDepth,
		const int32 SourceSize,
		const int32 OutputSize,
		TArray<FColor>& OutPixels)
	{
		OutPixels.SetNumZeroed(
			OutputSize *
			OutputSize
		);

		const float BackgroundDepth =
			GetBackgroundDepth(
				SourceDepth,
				SourceSize
			);

		constexpr int32 SamplesPerPixel =
			SuperSample * SuperSample;

		for (int32 Y = 0; Y < OutputSize; ++Y)
		{
			for (int32 X = 0; X < OutputSize; ++X)
			{
				FLinearColor ColorSum =
					FLinearColor::Black;

				int32 MeshSamples = 0;

				for (
					int32 SampleY = 0;
					SampleY < SuperSample;
					++SampleY)
				{
					for (
						int32 SampleX = 0;
						SampleX < SuperSample;
						++SampleX)
					{
						const int32 SourceX =
							X * SuperSample +
							SampleX;

						const int32 SourceY =
							Y * SuperSample +
							SampleY;

						const int32 SourceIndex =
							SourceY *
							SourceSize +
							SourceX;

						const float Depth =
							SourceDepth[
								SourceIndex
							].R;

						if (!IsMeshPixel(
							Depth,
							BackgroundDepth))
						{
							continue;
						}

						/*
						 * ReadPixels gave us actual sRGB bytes.
						 *
						 * Convert once to linear so averaging
						 * multiple supersamples is correct.
						 */
						const FLinearColor SampleColor =
							FLinearColor::FromSRGBColor(
								SourceColor[
									SourceIndex
								]
							);

						ColorSum += SampleColor;

						++MeshSamples;
					}
				}

				const int32 DestIndex =
					Y *
					OutputSize +
					X;

				if (MeshSamples == 0)
				{
					OutPixels[DestIndex] =
						FColor(
							0,
							0,
							0,
							0
						);

					continue;
				}

				/*
				 * IMPORTANT:
				 *
				 * Average only OBJECT samples.
				 *
				 * We deliberately don't include black
				 * background pixels in RGB.
				 *
				 * This is what removes the black outline.
				 */
				FLinearColor FinalColor =
					ColorSum /
					static_cast<float>(
						MeshSamples
						);

				/*
				 * Alpha represents pixel coverage.
				 */
				FinalColor.A =
					static_cast<float>(
						MeshSamples
						) /
					static_cast<float>(
						SamplesPerPixel
						);

				OutPixels[DestIndex] =
					FinalColor.ToFColorSRGB();
			}
		}
	}

	// ============================================================
	// Render mesh
	// ============================================================

	bool RenderMesh(
		UStaticMesh* StaticMesh,
		const int32 OutputSize,
		TArray<FColor>& OutPixels)
	{
		if (!IsValid(StaticMesh))
		{
			return false;
		}

		const int32 RenderSize =
			OutputSize *
			SuperSample;

		// ========================================================
		// Preview scene
		// ========================================================

		FPreviewScene PreviewScene(
			FPreviewScene::ConstructionValues()
			.SetCreatePhysicsScene(false)

			/*
			 * Keep REAL Unreal lighting.
			 *
			 * This means textures, vertex colors,
			 * material parameters, roughness etc.
			 * all remain intact.
			 */
			.SetCreateDefaultLighting(true)

			.SetLightBrightness(
				KeyLightIntensity
			)

			.SetLightRotation(
				FRotator(
					-35.0f,
					-45.0f,
					0.0f
				)
			)

			.SetSkyBrightness(
				SkyBrightness
			)

			.AllowAudioPlayback(false)
			.SetTransactional(false)
		);

		// ========================================================
		// Fill lights
		// ========================================================

		/*
		 * Opposite-side fill.
		 *
		 * Prevents half of the model falling into black.
		 */
		UDirectionalLightComponent* FillLight =
			AddFillLight(
				PreviewScene,
				FRotator(
					-25.0f,
					135.0f,
					0.0f
				),
				FillLightIntensity
			);

		/*
		 * Soft top/back light.
		 */
		UDirectionalLightComponent* BackLight =
			AddFillLight(
				PreviewScene,
				FRotator(
					-65.0f,
					45.0f,
					0.0f
				),
				BackLightIntensity
			);

		if (!FillLight ||
			!BackLight)
		{
			return false;
		}

		// ========================================================
		// Static Mesh
		// ========================================================

		UStaticMeshComponent* MeshComponent =
			NewObject<UStaticMeshComponent>(
				GetTransientPackage(),
				NAME_None,
				RF_Transient
			);

		if (!MeshComponent)
		{
			return false;
		}

		MeshComponent->SetStaticMesh(
			StaticMesh
		);

		MeshComponent->SetCollisionEnabled(
			ECollisionEnabled::NoCollision
		);

		/*
		 * Very important for icons:
		 *
		 * We still get normal-based light/shading,
		 * but the mesh does not cast deep shadows
		 * onto itself.
		 */
		MeshComponent->SetCastShadow(false);

		const FBoxSphereBounds Bounds =
			StaticMesh->GetBounds();

		PreviewScene.AddComponent(
			MeshComponent,
			FTransform(
				FRotator::ZeroRotator,
				-Bounds.Origin,
				FVector::OneVector
			)
		);

		// ========================================================
		// Scene Capture
		// ========================================================

		USceneCaptureComponent2D* CaptureComponent =
			NewObject<USceneCaptureComponent2D>(
				GetTransientPackage(),
				NAME_None,
				RF_Transient
			);

		if (!CaptureComponent)
		{
			return false;
		}

		CaptureComponent->bCaptureEveryFrame =
			false;

		CaptureComponent->bCaptureOnMovement =
			false;

		CaptureComponent->ProjectionType =
			ECameraProjectionMode::Perspective;

		CaptureComponent->FOVAngle =
			ThumbnailFOV;

		CaptureComponent->PrimitiveRenderMode =
			ESceneCapturePrimitiveRenderMode
			::PRM_UseShowOnlyList;

		CaptureComponent->ShowOnlyComponent(
			MeshComponent
		);

		// ========================================================
		// Stable exposure
		// ========================================================

		CaptureComponent->PostProcessBlendWeight =
			1.0f;

		CaptureComponent
			->PostProcessSettings
			.bOverride_AutoExposureMethod =
			true;

		CaptureComponent
			->PostProcessSettings
			.AutoExposureMethod =
			EAutoExposureMethod::AEM_Manual;

		CaptureComponent
			->PostProcessSettings
			.bOverride_AutoExposureApplyPhysicalCameraExposure =
			true;

		CaptureComponent
			->PostProcessSettings
			.AutoExposureApplyPhysicalCameraExposure =
			false;

		CaptureComponent
			->PostProcessSettings
			.bOverride_AutoExposureBias =
			true;

		CaptureComponent
			->PostProcessSettings
			.AutoExposureBias =
			0.0f;

		/*
		 * Disable AO.
		 *
		 * AO is useful in a world scene, but for a tiny item
		 * icon it often produces ugly nearly-black creases.
		 */
		CaptureComponent
			->PostProcessSettings
			.bOverride_AmbientOcclusionIntensity =
			true;

		CaptureComponent
			->PostProcessSettings
			.AmbientOcclusionIntensity =
			0.0f;

		// ========================================================
		// Show flags
		// ========================================================

		/*
		 * AA disabled deliberately.
		 *
		 * We get clean AA by supersampling instead.
		 */
		CaptureComponent
			->ShowFlags
			.SetAntiAliasing(false);

		CaptureComponent
			->ShowFlags
			.SetMotionBlur(false);

		CaptureComponent
			->ShowFlags
			.SetFog(false);

		CaptureComponent
			->ShowFlags
			.SetAtmosphere(false);

		CaptureComponent
			->ShowFlags
			.SetGrid(false);

		CaptureComponent
			->ShowFlags
			.SetSelectionOutline(false);

		// ========================================================
		// Camera
		// ========================================================

		FVector CameraLocation;
		FRotator CameraRotation;

		GetThumbnailCamera(
			StaticMesh,
			CameraLocation,
			CameraRotation
		);

		PreviewScene.AddComponent(
			CaptureComponent,
			FTransform(
				CameraRotation,
				CameraLocation,
				FVector::OneVector
			)
		);

		MeshComponent->MarkRenderStateDirty();

		FillLight->MarkRenderStateDirty();

		BackLight->MarkRenderStateDirty();

		CaptureComponent->MarkRenderStateDirty();

		FlushRenderingCommands();

		// ========================================================
		// COLOR PASS
		// ========================================================

		UTextureRenderTarget2D* ColorTarget =
			CreateColorRenderTarget(
				RenderSize
			);

		if (!ColorTarget)
		{
			return false;
		}

		CaptureComponent->TextureTarget =
			ColorTarget;

		/*
		 * THIS is the major change.
		 *
		 * Render final Unreal material result.
		 *
		 * No BaseColor extraction.
		 * No reconstruction from normals.
		 * No manual fake material coloring.
		 */
		CaptureComponent->CaptureSource =
			ESceneCaptureSource::SCS_FinalColorLDR;

		CaptureComponent->MarkRenderStateDirty();

		CaptureComponent->CaptureScene();

		FlushRenderingCommands();

		TArray<FColor> ColorPixels;

		if (!ReadColorPixels(
			ColorTarget,
			ColorPixels))
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT(
					"Failed to read color target"
				)
			);

			return false;
		}

		// ========================================================
		// DEPTH PASS
		// ========================================================

		UTextureRenderTarget2D* DepthTarget =
			CreateDepthRenderTarget(
				RenderSize
			);

		if (!DepthTarget)
		{
			return false;
		}

		CaptureComponent->TextureTarget =
			DepthTarget;

		/*
		 * Depth has exactly one purpose:
		 *
		 * Is there mesh at this pixel or not?
		 *
		 * It never affects RGB.
		 */
		CaptureComponent->CaptureSource =
			ESceneCaptureSource::SCS_SceneDepth;

		CaptureComponent->MarkRenderStateDirty();

		CaptureComponent->CaptureScene();

		FlushRenderingCommands();

		TArray<FLinearColor> DepthPixels;

		if (!ReadDepthPixels(
			DepthTarget,
			DepthPixels))
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT(
					"Failed to read depth target"
				)
			);

			return false;
		}

		const int32 ExpectedPixelCount =
			RenderSize *
			RenderSize;

		if (
			ColorPixels.Num() !=
			ExpectedPixelCount ||
			DepthPixels.Num() !=
			ExpectedPixelCount)
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT(
					"Invalid render target size"
				)
			);

			return false;
		}

		// ========================================================
		// Transparent PNG pixels
		// ========================================================

		Downsample(
			ColorPixels,
			DepthPixels,
			RenderSize,
			OutputSize,
			OutPixels
		);

		return true;
	}
}

#endif // WITH_EDITOR


// ================================================================
// Export
// ================================================================

bool UStaticMeshThumbnailLibrary::ExportStaticMeshThumbnail(
	UStaticMesh* StaticMesh,
	const FString& OutputDirectory,
	const int32 Size,
	FString& OutSavedPath)
{
#if WITH_EDITOR

	OutSavedPath.Empty();

	if (!IsValid(StaticMesh))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"%hs: StaticMesh is invalid"
			),
			__FUNCTION__
		);

		return false;
	}

	if (Size <= 0)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"%hs: Invalid size: %d"
			),
			__FUNCTION__,
			Size
		);

		return false;
	}

	// ============================================================
	// Output directory
	// ============================================================

	FString Directory =
		OutputDirectory;

	if (Directory.IsEmpty())
	{
		Directory =
			FPaths::ProjectSavedDir() /
			TEXT("GeneratedIcons");
	}

	Directory =
		FPaths::ConvertRelativePathToFull(
			Directory
		);

	IFileManager::Get().MakeDirectory(
		*Directory,
		true
	);

	const FString OutputPath =
		Directory /
		(
			StaticMesh->GetName() +
			TEXT(".png")
			);

	// ============================================================
	// Render
	// ============================================================

	TArray<FColor> Pixels;

	if (!StaticMeshThumbnailExporter::RenderMesh(
		StaticMesh,
		Size,
		Pixels))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"%hs: Failed to render %s"
			),
			__FUNCTION__,
			*StaticMesh->GetName()
		);

		return false;
	}

	if (Pixels.Num() !=
		Size * Size)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"%hs: Invalid pixel count"
			),
			__FUNCTION__
		);

		return false;
	}

	// ============================================================
	// PNG
	// ============================================================

	/*
	 * Pixels contain sRGB RGB + linear alpha.
	 */
	const FImageView Image(
		Pixels.GetData(),
		Size,
		Size,
		EGammaSpace::sRGB
	);

	TArray64<uint8> PNGData;

	if (!FImageUtils::CompressImage(
		PNGData,
		TEXT("png"),
		Image,
		100))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"%hs: PNG compression failed"
			),
			__FUNCTION__
		);

		return false;
	}

	if (!FFileHelper::SaveArrayToFile(
		PNGData,
		*OutputPath))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"%hs: Failed to save: %s"
			),
			__FUNCTION__,
			*OutputPath
		);

		return false;
	}

	OutSavedPath =
		OutputPath;

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"Static Mesh icon exported: %s"
		),
		*OutputPath
	);

	return true;

#else

	OutSavedPath.Empty();

	return false;

#endif
}