#include "Settings/ZombieGameUserSettings.h"

#include "AudioDevice.h"
#include "Engine.h"
#include "Engine/World.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "Sound/SoundMix.h"
#include "Settings/CustomSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogGameUserSettings, Log, All);

bool IsWithEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}

UWorld* UZombieGameUserSettings::GetWorld() const
{
	if (!GEngine)
	{
		return nullptr;
	}
	if (!GEngine->GameViewport)
	{
		return nullptr;
	}
	return GEngine->GameViewport->GetWorld();
}

UZombieGameUserSettings* UZombieGameUserSettings::GetZombieUserSettings()
{
	return Cast<UZombieGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UZombieGameUserSettings::ChangeSoundValue(float NewValue, UObject* WorldRef, USoundClass* ActiveSoundClass)
{
	if (!SoundMix)
	{
		return;
	}

	ApplySoundSettings(NewValue, WorldRef, ActiveSoundClass);
}

void UZombieGameUserSettings::SetSoundValue(EAvailableSoundVolumes Type, float NewValue)
{
	UISoundValue = NewValue;
	ChangeSoundValue(NewValue, this, UISoundClass);

	SaveSettings();
}

void UZombieGameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);

	LoadSoundParameters();

}

void UZombieGameUserSettings::LoadSoundParameters()
{
	const UCustomSettings* Settings = GetDefault<UCustomSettings>();
	if (Settings)
	{
		SoundMix = Settings->SoundSettings.SoundMix;
		UISoundClass = Settings->SoundSettings.UISoundClass;
	}

	ChangeSoundValue(UISoundValue, this, UISoundClass);
}

void UZombieGameUserSettings::ApplySoundSettings(float Value, UObject* WorldRef, class USoundClass* ActiveSoundClass)
{
	if (!GetWorld())
	{
		return;
	}

	UGameplayStatics::SetSoundMixClassOverride(WorldRef, SoundMix, ActiveSoundClass, 2.f * Value, 1, 0.f);
	UGameplayStatics::PushSoundMixModifier(WorldRef, SoundMix);
}
