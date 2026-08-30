// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedMVVM/Viewmodels/BaseViewModel.h"
#include "AdvancedMVVM/Definitions/ViewModelDefinitions.h"
#include "CafeDefinitions.h"
#include "CafeViewModel.generated.h"

UCLASS()
class GAME_API UCafeViewModel : public UBaseViewModel
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, FieldNotify)
	EInteractionType GetInteractionType() const { return Type; }

protected:
	// Begin of UBaseViewModel override
	virtual void ViewModelInitialize(AActor* InActor) override;
	virtual void ViewModelClear() override;
	//~End of UBaseViewModel override

	void HandleOnPlayerInteraction(AActor* InteractedActor);

private:
	void InitManagersBindings();
	void ClearManagersBindings();

	EInteractionType Type;
};
