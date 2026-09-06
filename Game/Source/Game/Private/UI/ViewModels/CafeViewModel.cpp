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
	FGMPHelper::ListenWorldMessage(GetWorld(), MSGKEY("GMP.OnStorageUpdated"), this, &ThisClass::HandleOnStorageUpdated);
}

void UCafeViewModel::ClearManagersBindings()
{
	FGMPHelper::UnbindMessage(MSGKEY("GMP.OnStorageUpdated"), this, &ThisClass::HandleOnStorageUpdated);
	FGMPHelper::UnbindMessage(MSGKEY("GMP.OnInteractionClicked"), this, &ThisClass::HandleOnPlayerInteraction);
}

void UCafeViewModel::HandleOnPlayerInteraction(FHitResult HitResult, const TArray<AActor*>& InteractedActors)
{
	const auto InteractedActor = InteractedActors.FindByPredicate([](const AActor* Actor) { return IsValid(Actor) && Actor->Implements<UInteractionInterface>(); });

	if (InteractedActor)
	{
		ActorForInteracted = *InteractedActor;
		Type = IInteractionInterface::Execute_GetInteractionType(ActorForInteracted);

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetInteractionType);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActorForInteracted);
	}
}

void UCafeViewModel::HandleOnStorageUpdated(AActor* InteractedActor)
{
	if (ActorForInteracted == InteractedActor)
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActorForInteracted);
	}
}
