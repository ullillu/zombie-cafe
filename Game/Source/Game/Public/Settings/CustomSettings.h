#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CustomSettings.generated.h"

class UUserWidget;

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Custom Game Settings"))
class GAME_API UCustomSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    virtual FName GetCategoryName() const override;

    UPROPERTY(EditAnywhere, Config, Category = "UI")
    TSoftClassPtr<UUserWidget> LoadingWidgetClass;

};

inline FName UCustomSettings::GetCategoryName() const
{
    return TEXT("Zombie UI Settings");
}
