// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/ViewModels/CafeViewModel.h"
#include "InteractionInterface.h"
#include "GMPHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CafeViewModel)

void UCafeViewModel::ViewModelInitialize(AActor* InActor)
{
	Super::ViewModelInitialize(InActor);

	InitManagersBindings();
}

void UCafeViewModel::ViewModelClear()
{
	ClearManagersBindings();

	Super::ViewModelClear();
}

void UCafeViewModel::InitManagersBindings()
{
	FGMPHelper::ListenWorldMessage(GetWorld(), MSGKEY("GMP.OnInteractionClicked"), this, &ThisClass::HandleOnPlayerInteraction);
}

void UCafeViewModel::ClearManagersBindings()
{
	FGMPHelper::UnbindMessage(MSGKEY("GMP.OnInteractionClicked"), this, &ThisClass::HandleOnPlayerInteraction);
}

void UCafeViewModel::HandleOnPlayerInteraction(AActor* InteractedActor)
{
	if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(InteractedActor))
	{
		Type = InteractionInterface->Execute_GetInteractionType(InteractedActor);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetInteractionType);
	}
}
