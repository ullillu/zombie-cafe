// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CafeDefinitions.generated.h"


UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None,
	StorageCupboard,

};

UENUM(BlueprintType)
enum class EFoodType : uint8
{
	None,
	Appetizer,
	Salad,
	Soup,
	MainDish,
	Grill,
	Pizza,
	Dessert,
	Vegan,
	ReadyFood,
};

UENUM(BlueprintType)
enum class EDrinkType : uint8
{
	None,
	Alcohol,
	Lemonade,
	Cofe,
	Tea,
};
