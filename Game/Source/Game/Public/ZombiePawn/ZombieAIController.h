// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AZombieAIController : public AAIController
{
	GENERATED_BODY()

public:
	AZombieAIController();

	UFUNCTION(BlueprintCallable, Category = "Zombie|AI")
	void MoveZombieToLocation(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Zombie|AI")
	void MoveZombieToActor(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Zombie|AI")
	void StopZombieMovement();


protected:
	virtual void OnPossess(APawn* InPawn) override;
	
};
