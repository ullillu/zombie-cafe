// Copyright (c) 2024 KomBez Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

#include "ZombieGameUserSettings.generated.h"

class USoundMix;

UENUM(BlueprintType)
enum class EAvailableSoundVolumes : uint8
{
	Master = 0 UMETA(DisplayName = "Master"),
	UI = 1 UMETA(DisplayName = "UI")
};

UCLASS(BlueprintType)
class GAME_API UZombieGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	virtual class UWorld* GetWorld() const override;

	UFUNCTION(BlueprintPure)
	static UZombieGameUserSettings* GetZombieUserSettings();

	void LoadSoundParameters();

	UFUNCTION(BlueprintCallable)
	void ChangeSoundValue(float NewValue, UObject* WorldRef, class USoundClass* ActiveSoundClass);

	UFUNCTION(BlueprintCallable)
	void SetSoundValue(EAvailableSoundVolumes Type, float NewValue);

	UFUNCTION(BlueprintPure)
	float GetUISoundValue() const { return UISoundValue; }

public:

	virtual void LoadSettings(bool bForceReload = false);

protected:
	void ApplySoundSettings(float Value, UObject* WorldRef, class USoundClass* ActiveSoundClass);

protected:

	UPROPERTY(config)
	float UISoundValue = 0.5f;

private:

	UPROPERTY()
	TObjectPtr<USoundMix> SoundMix;

	UPROPERTY()
	TObjectPtr<USoundClass> UISoundClass;
};
