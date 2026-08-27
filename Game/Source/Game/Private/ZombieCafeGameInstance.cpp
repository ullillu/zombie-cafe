// Fill out your copyright notice in the Description page of Project Settings.
#include "ZombieCafeGameInstance.h"
#include "MoviePlayer.h"
#include "Settings/CustomSettings.h"
#include "Settings/ZombieGameUserSettings.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "ZombieSaveGame.h"

DEFINE_LOG_CATEGORY(LogZombieGameInstance);

void UZombieCafeGameInstance::CreateLoadingScreen(const FString& LevelName)
{
	const UCustomSettings* Settings = GetDefault<UCustomSettings>();
	if (Settings == nullptr)
	{
		UE_LOG(LogZombieGameInstance, Error, TEXT("%hs :: Settings must be valid!"), __FUNCTION__);
		return;
	}

	auto MoviePlayer = GetMoviePlayer();
	if (MoviePlayer == nullptr)
	{
		UE_LOG(LogZombieGameInstance, Error, TEXT("%hs :: MoviePlayer must be valid!"), __FUNCTION__);
		return;
	}

	auto LoadingScreenWidgetClass = Settings->LoadingWidgetClass.LoadSynchronous();
	if (LoadingScreenWidgetClass == nullptr)
	{
		UE_LOG(LogZombieGameInstance, Error, TEXT("%hs :: LoadingScreenWidgetClass must be valid!"), __FUNCTION__);
		return;
	}

	FLoadingScreenAttributes LoadingScreen;

	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;

	auto* LoadingScreenWidget = CreateWidget(GetWorld(), LoadingScreenWidgetClass);

	if (LoadingScreenWidget == nullptr)
	{
		UE_LOG(LogZombieGameInstance, Error, TEXT("%hs :: LoadingScreenWidget must be valid!"), __FUNCTION__);
		return;
	}

	LoadingScreen.WidgetLoadingScreen = LoadingScreenWidget->TakeWidget();
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UZombieCafeGameInstance::LoadMapCompleted(UWorld* Level)
{
	if (SaveGame.IsValid() == false)
	{
		SaveGame = UZombieSaveGame::LoadZombieGame();
	}

	auto* UserSettings = UZombieGameUserSettings::GetZombieUserSettings(); 
	if (UserSettings == nullptr)
	{
		UE_LOG(LogZombieGameInstance, Error, TEXT("%hs :: UserSettings must be valid!"), __FUNCTION__);
		return;
	}

	UserSettings->LoadSettings(false);
}

void UZombieCafeGameInstance::Init()
{
	Super::Init();
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::CreateLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::LoadMapCompleted);
}

void UZombieCafeGameInstance::Shutdown()
{
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	SaveGame.Reset();
	Super::Shutdown();
}

TObjectPtr<UZombieSaveGame> UZombieCafeGameInstance::GetSaveGame() const
{
	return SaveGame.Get();
}
