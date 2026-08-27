// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ZombieCafeGameInstance.generated.h"

class UWorld;
class UZombieSaveGame;

DECLARE_LOG_CATEGORY_EXTERN(LogZombieGameInstance, Log, All);

UCLASS()
class UZombieCafeGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	void CreateLoadingScreen(const FString& LevelName);
	void LoadMapCompleted(UWorld* Level);

	virtual void Init() override;
	virtual void Shutdown() override;

	TObjectPtr<UZombieSaveGame> GetSaveGame() const;

private:

	TWeakObjectPtr<UZombieSaveGame> SaveGame;

};
