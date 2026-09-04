// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CafeDefinitions.h"
#include "Slate/WidgetTransform.h"
#include "FoodRowBase.generated.h"

class UStaticMesh;
class UTexture2D;

USTRUCT(BlueprintType)
struct FReadyProductData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> ReadyProductIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> ReadyProductMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EFoodType> FoodTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EDrinkType> DrinkTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ProductLimit = 1;
};

USTRUCT()
struct FFoodRowBase : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FReadyProductData ReadyProductData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsBase = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsAdditive = false;

};
