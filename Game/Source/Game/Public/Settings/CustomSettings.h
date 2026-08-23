#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CustomSettings.generated.h"

class UUserWidget;
class USoundMix;
class USoundClass;

USTRUCT(Blueprintable)
struct GAME_API FRuntimeMusicSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	USoundMix* SoundMix = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	USoundClass* UISoundClass = nullptr;

};

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Custom Game Settings"))
class GAME_API UCustomSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    virtual FName GetCategoryName() const override;

    UPROPERTY(EditAnywhere, Config, Category = "UI")
    TSoftClassPtr<UUserWidget> LoadingWidgetClass;

    UPROPERTY(EditAnywhere, Config, Category = "Sound")
    FRuntimeMusicSettings SoundSettings;
};

inline FName UCustomSettings::GetCategoryName() const
{
    return TEXT("Zombie UI Settings");
}
