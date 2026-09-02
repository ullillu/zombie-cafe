// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombiePawn/ZombieAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

AZombieAIController::AZombieAIController()
{
	bAttachToPawn = true;
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ensure(InPawn);
}

void AZombieAIController::MoveZombieToLocation(const FVector& Location)
{
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(Location);
    MoveRequest.SetAcceptanceRadius(20.f);
    MoveRequest.SetUsePathfinding(true);
    MoveRequest.SetAllowPartialPath(true);
    MoveRequest.SetProjectGoalLocation(true);
    
    const FPathFollowingRequestResult Result = MoveTo(MoveRequest);
}

void AZombieAIController::MoveZombieToActor(AActor* TargetActor)
{
    if (!IsValid(TargetActor))
    {
        return;
    }

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(TargetActor);
    MoveRequest.SetAcceptanceRadius(50.f);
    MoveRequest.SetUsePathfinding(true);
    MoveRequest.SetAllowPartialPath(true);

    MoveTo(MoveRequest);
}

void AZombieAIController::StopZombieMovement()
{
    StopMovement();
}


