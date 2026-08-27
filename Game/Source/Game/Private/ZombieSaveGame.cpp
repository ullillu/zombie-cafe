// Fill out your copyright notice in the Description page of Project Settings.
#include "ZombieSaveGame.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	static FString SaveSlotName = "ZombieSave";
	static uint32 UserIndex = 0;
}

void UZombieSaveGame::SaveZombieGame()
{
	if (UZombieSaveGame* SaveGameInstance = Cast<UZombieSaveGame>(UGameplayStatics::CreateSaveGameObject(UZombieSaveGame::StaticClass())))
	{
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, UserIndex);		
	}
}

UZombieSaveGame* UZombieSaveGame::LoadZombieGame()
{
	return Cast<UZombieSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
}

void UZombieSaveGame::AddNewCupboardData(const FString& StorageName, const FStorageCupboardData& NewData)
{
	if (FStorageCupboardSaveData* Found = StorageCupboardSaveData.FindByPredicate([StorageName](const FStorageCupboardSaveData& Other) { return Other.StorageName.Equals(StorageName, ESearchCase::IgnoreCase); }))
	{
		Found->Data = NewData;
	}
	else
	{
		StorageCupboardSaveData.Add({ StorageName , NewData });
	}
}

bool UZombieSaveGame::GetCupboardData(const FString& StorageName, FStorageCupboardData& Data)
{
	if (FStorageCupboardSaveData* Found = StorageCupboardSaveData.FindByPredicate([StorageName](const FStorageCupboardSaveData& Other) { return Other.StorageName.Equals(StorageName, ESearchCase::IgnoreCase); }))
	{
		Data = Found->Data;
		return true;
	}
	return false;
}
