// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ZombiePawnBase.generated.h"

class UCapsuleComponent;
class UZombieMovementComponent;
class UStaticMeshComponent;

UCLASS()
class GAME_API AZombiePawnBase : public APawn
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

protected:
	void DeselectZombie();
	void HandleOnPlayerInteraction(AActor* InteractedActor, FHitResult HitResult);

	void MoveZombieToClickPosition(FHitResult HitResult);
public:	
	AZombiePawnBase();

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie")
	TObjectPtr<UZombieMovementComponent> ZombieMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie")
	TObjectPtr<UStaticMeshComponent> PlaneMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie")
	bool bSelected = false;

	void SetSelected(bool Selected);
};
