// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StorageComponent.h"
#include "ZombieCafeGameInstance.h"
#include "ZombieSaveGame.h"

// Sets default values for this component's properties
UStorageComponent::UStorageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UStorageComponent::AddItemToCupboard(const FStorageCupboardProductsData& Product)
{
	FStorageCupboardProductsData* ProductData = Data.Products.FindByPredicate([Product](const FStorageCupboardProductsData& Other) { return Other.ProductId.Equals(Product.ProductId, ESearchCase::IgnoreCase); });
	if (ProductData)
	{
		ProductData->Count += Product.Count;
		return true;
	}

	if ((ShelfCapacity * Data.AvailableShelves) > Data.Products.Num())
	{
		Data.Products.Add(Product); 
		return true;
	}
	return false;
}

bool UStorageComponent::PopItemFromCupboard(const FString& ProductId)
{
	int32 Index = Data.Products.IndexOfByPredicate([ProductId](const FStorageCupboardProductsData& Other) { return Other.ProductId.Equals(ProductId, ESearchCase::IgnoreCase); });
	if (Index != INDEX_NONE)
	{
		Data.Products[Index].Count--;
		if (Data.Products[Index].Count <= 0)
		{
			Data.Products.RemoveAt(Index);
			return true;
		}
	}
	return false;
}

void UStorageComponent::SaveItemsInCupboard()
{

	if (GetWorld() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs :: GetWorld must be valid!"), __FUNCTION__);
		return;
	}
	auto* GI = Cast<UZombieCafeGameInstance>(GetWorld()->GetGameInstance());
	if (GI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs :: GetGameInstance must be valid!"), __FUNCTION__);
		return;
	}

	UZombieSaveGame* SaveGame = GI->GetSaveGame();
	if (SaveGame == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs :: SaveGame must be valid!"), __FUNCTION__);
		return;
	}

	SaveGame->AddNewCupboardData(GetName(), Data);
}


// Called when the game starts
void UStorageComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs :: GetWorld must be valid!"), __FUNCTION__);
		return;
	}
	auto* GI = Cast<UZombieCafeGameInstance>(GetWorld()->GetGameInstance());
	if (GI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs :: GetGameInstance must be valid!"), __FUNCTION__);
		return;
	}

	UZombieSaveGame* SaveGame = GI->GetSaveGame();
	if (SaveGame == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs :: SaveGame must be valid!"), __FUNCTION__);
		return;
	}

	if (SaveGame->GetCupboardData(GetName(), Data) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("%hs :: can't find data to %s!"), __FUNCTION__, *GetName());
	}
}


// Called every frame
void UStorageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

