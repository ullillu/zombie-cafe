// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponents.generated.h"

class APlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UInteractionComponents : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponents();

	UFUNCTION(BlueprintPure)
	TArray<AActor*> GetActorsAtMousePosition(APlayerController* PC, float Radius = 300.0f);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		
};
