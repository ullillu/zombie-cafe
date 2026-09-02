// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractionComponents.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"

// Sets default values for this component's properties
UInteractionComponents::UInteractionComponents()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

TArray<AActor*> UInteractionComponents::GetActorsAtMousePosition(APlayerController* PC, float Radius)
{
	TArray<AActor*> ResultActors;

	if (PC == nullptr)
	{
		return ResultActors;
	}
	
	FVector WorldLocation;
	FVector WorldDirection;

	if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection) == false)
	{
		return ResultActors;
	}

	const FVector TraceStart = WorldLocation;
	const FVector TraceEnd = TraceStart + WorldDirection * 100000.0f;

	FHitResult HitResult;

	FCollisionQueryParams TraceParams;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		TraceParams
	);

	if (!bHit)
	{
		return ResultActors;
	}

	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;
	//ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	//ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
	//ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FCollisionQueryParams OverlapParams;

	const FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

	GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		HitResult.ImpactPoint,
		FQuat::Identity,
		ObjectQueryParams,
		Sphere,
		OverlapParams
	);

	// 4. OverlapResult -> TArray<AActor*>
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* Actor = Overlap.GetActor();

		if (IsValid(Actor))
		{
			ResultActors.AddUnique(Actor);
		}
	}

	return ResultActors;
}


// Called when the game starts
void UInteractionComponents::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInteractionComponents::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

