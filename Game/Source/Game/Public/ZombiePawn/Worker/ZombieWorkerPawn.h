// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ZombiePawnBase.h"
#include "ZombieWorkerPawn.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AZombieWorkerPawn : public AZombiePawnBase
{
	GENERATED_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 HandsCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 LegsCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MaxCarriedWeight = 20.f;



	
};
