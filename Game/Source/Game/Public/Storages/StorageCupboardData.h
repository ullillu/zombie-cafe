// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StorageCupboardData.generated.h"

class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FStorageCupboardProductsData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ProductId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count;

};

USTRUCT(BlueprintType)
struct FStorageCupboardData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AvailableShelves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FStorageCupboardProductsData> Products;

};
