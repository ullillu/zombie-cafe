// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ZombieBPFL.generated.h"

/**
 *
 */
UCLASS()
class GAME_API UStaticMeshThumbnailLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Editor|Thumbnail", meta = (DevelopmentOnly))
	static bool ExportStaticMeshThumbnail(UStaticMesh* StaticMesh, const FString& OutputDirectory, int32 Size, FString& OutSavedPath);
};
