#include "RenderTargetDiskActor.h"

#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "ImageCore.h"
#include "ImageUtils.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY_STATIC(LogRenderTargetDiskActor, Log, All);

ARenderTargetDiskActor::ARenderTargetDiskActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ARenderTargetDiskActor::SaveFileOnDisk(const FString& FileName)
{
	if (!IsValid(RenderTarget))
	{
		UE_LOG(LogRenderTargetDiskActor, Error,
			TEXT("SaveFileOnDisk failed: RenderTarget is not assigned."));
		return false;
	}

	const FString SafeBaseName = MakeSafeBaseFileName(FileName);
	if (SafeBaseName.IsEmpty())
	{
		UE_LOG(LogRenderTargetDiskActor, Error,
			TEXT("SaveFileOnDisk failed: FileName is empty or invalid."));
		return false;
	}

	const FString SaveDirectory = GetSaveDirectory();
	IFileManager& FileManager = IFileManager::Get();

	if (!FileManager.DirectoryExists(*SaveDirectory) &&
		!FileManager.MakeDirectory(*SaveDirectory, true))
	{
		UE_LOG(LogRenderTargetDiskActor, Error,
			TEXT("SaveFileOnDisk failed: could not create directory '%s'."),
			*SaveDirectory);
		return false;
	}

	// This performs a synchronous GPU readback. Call it on demand, not every frame.
	FImage RenderTargetImage;
	if (!FImageUtils::GetRenderTargetImage(RenderTarget, RenderTargetImage))
	{
		UE_LOG(LogRenderTargetDiskActor, Error,
			TEXT("SaveFileOnDisk failed: could not read RenderTarget."));
		return false;
	}

	// PNG is an 8-bit format. This also makes saving float render targets
	// deterministic instead of switching the output format to EXR.
	RenderTargetImage.ChangeFormat(
		ERawImageFormat::BGRA8,
		EGammaSpace::sRGB);

	if (bForceOpaqueAlpha)
	{
		for (FColor& Pixel : RenderTargetImage.AsBGRA8())
		{
			Pixel.A = 255;
		}
	}

	TArray64<uint8> PngData;
	if (!FImageUtils::CompressImage(PngData, TEXT("png"), RenderTargetImage, 100))
	{
		UE_LOG(LogRenderTargetDiskActor, Error,
			TEXT("SaveFileOnDisk failed: PNG encoding failed."));
		return false;
	}

	const FString FullPath = FPaths::Combine(
		SaveDirectory,
		SafeBaseName + TEXT(".png"));

	if (!FFileHelper::SaveArrayToFile(PngData, *FullPath))
	{
		UE_LOG(LogRenderTargetDiskActor, Error,
			TEXT("SaveFileOnDisk failed: could not write '%s'."),
			*FullPath);
		return false;
	}

	// Drop an older in-memory copy so the next load definitely reads the new file.
	LoadedTextures.Remove(FullPath);

	UE_LOG(LogRenderTargetDiskActor, Log,
		TEXT("Render Target saved to '%s'."),
		*FullPath);
	return true;
}

UTexture2D* ARenderTargetDiskActor::LoadFileFromDisk(const FString& FileName)
{
	const FString FullPath = GetFilePath(FileName);
	if (FullPath.IsEmpty() || !FPaths::FileExists(FullPath))
	{
		UE_LOG(LogRenderTargetDiskActor, Warning,
			TEXT("LoadFileFromDisk failed: file does not exist: '%s'."),
			*FullPath);
		return nullptr;
	}

	UTexture2D* LoadedTexture =
		UKismetRenderingLibrary::ImportFileAsTexture2D(this, FullPath);

	if (!IsValid(LoadedTexture))
	{
		UE_LOG(LogRenderTargetDiskActor, Error,
			TEXT("LoadFileFromDisk failed: could not decode '%s'."),
			*FullPath);
		return nullptr;
	}

	// ImportFileAsTexture2D creates a transient object. The UPROPERTY cache keeps
	// it referenced and prevents garbage collection while this actor is alive.
	LoadedTextures.Add(FullPath, LoadedTexture);
	return LoadedTexture;
}

bool ARenderTargetDiskActor::LoadFileIntoImage(
	const FString& FileName,
	UImage* TargetImage,
	const bool bMatchWidgetSize)
{
	if (!IsValid(TargetImage))
	{
		UE_LOG(LogRenderTargetDiskActor, Error,
			TEXT("LoadFileIntoImage failed: TargetImage is invalid."));
		return false;
	}

	UTexture2D* LoadedTexture = LoadFileFromDisk(FileName);
	if (!IsValid(LoadedTexture))
	{
		return false;
	}

	TargetImage->SetBrushFromTexture(LoadedTexture, bMatchWidgetSize);
	return true;
}

FString ARenderTargetDiskActor::GetFilePath(const FString& FileName) const
{
	const FString SafeBaseName = MakeSafeBaseFileName(FileName);
	if (SafeBaseName.IsEmpty())
	{
		return FString();
	}

	return FPaths::Combine(
		GetSaveDirectory(),
		SafeBaseName + TEXT(".png"));
}

FString ARenderTargetDiskActor::GetSaveDirectory() const
{
	return FPaths::Combine(
		FPaths::ProjectSavedDir(),
		TEXT("RenderTargetImages"));
}

FString ARenderTargetDiskActor::MakeSafeBaseFileName(const FString& FileName)
{
	// GetCleanFilename prevents callers from escaping the dedicated Saved folder.
	FString CleanName = FPaths::GetCleanFilename(FileName);
	CleanName = FPaths::GetBaseFilename(CleanName);
	CleanName.TrimStartAndEndInline();

	return CleanName.IsEmpty()
		? FString()
		: FPaths::MakeValidFileName(CleanName, TEXT('_'));
}
