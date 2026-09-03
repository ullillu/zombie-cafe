#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RenderTargetDiskActor.generated.h"

class UImage;
class UTexture2D;
class UTextureRenderTarget2D;

/**
 * Saves a render target to a PNG file and loads it back at runtime.
 *
 * Files are stored in:
 *   <Project Saved Directory>/RenderTargetImages/
 *
 * The implementation uses runtime APIs and works in packaged builds.
 */
UCLASS(BlueprintType, Blueprintable)
class GAME_API ARenderTargetDiskActor : public AActor
{
	GENERATED_BODY()

public:
	ARenderTargetDiskActor();

	/** Render target whose current contents will be saved. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Render Target|Disk")
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	/**
	 * Forces every saved PNG pixel to be opaque.
	 * SceneCapture and post-process render targets frequently contain zero alpha,
	 * which otherwise makes the loaded image invisible in UMG.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Render Target|Disk")
	bool bForceOpaqueAlpha = true;

	/**
	 * Saves the current contents of RenderTarget as a PNG.
	 * FileName may be passed with or without the .png extension.
	 */
	UFUNCTION(BlueprintCallable, Category = "Render Target|Disk")
	bool SaveFileOnDisk(const FString& FileName);

	/**
	 * Loads a previously saved PNG as a transient UTexture2D.
	 * Returns nullptr when the file cannot be found or decoded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Render Target|Disk")
	UTexture2D* LoadFileFromDisk(const FString& FileName);

	/** Loads a PNG and immediately assigns it to a UMG Image widget. */
	UFUNCTION(BlueprintCallable, Category = "Render Target|Disk")
	bool LoadFileIntoImage(
		const FString& FileName,
		UImage* TargetImage,
		bool bMatchWidgetSize = false);

	/** Returns the absolute PNG path used by save/load. */
	UFUNCTION(BlueprintPure, Category = "Render Target|Disk")
	FString GetFilePath(const FString& FileName) const;

private:
	FString GetSaveDirectory() const;
	static FString MakeSafeBaseFileName(const FString& FileName);

	/** Keeps runtime-loaded textures alive while this actor exists. */
	UPROPERTY(Transient)
	TMap<FString, TObjectPtr<UTexture2D>> LoadedTextures;
};
