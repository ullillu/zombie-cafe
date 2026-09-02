// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ZombieMovementComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent))
class GAME_API UZombieMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
    UZombieMovementComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;

    virtual void RequestPathMove(const FVector& MoveInput) override;

    virtual void StopActiveMovement() override;

    virtual float GetMaxSpeed() const override;

    virtual bool CanStartPathFollowing() const override;
    virtual bool CanStopPathFollowing() const override;

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Movement")
    float MaxSpeed = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Movement")
    float RotationSpeed = 360.f;

    FVector RequestedVelocity = FVector::ZeroVector;
};
