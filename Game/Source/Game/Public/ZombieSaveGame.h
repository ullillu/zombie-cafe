// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Storages/StorageCupboardData.h"
#include "ZombieSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FStorageCupboardSaveData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString StorageName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FStorageCupboardData Data;

};

UCLASS()
class GAME_API UZombieSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	static void SaveZombieGame();
	static UZombieSaveGame* LoadZombieGame();


	UFUNCTION(BlueprintCallable)
	void AddNewCupboardData(const FString& StorageName, const FStorageCupboardData& NewData);

	UFUNCTION(BlueprintPure)
	bool GetCupboardData(const FString& StorageName, FStorageCupboardData& Data);

protected:

	UPROPERTY()
	TArray<FStorageCupboardSaveData> StorageCupboardSaveData;

private:

};
