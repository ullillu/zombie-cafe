// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombiePawn/ZombiePawnBase.h"
#include "ZombiePawn/ZombieAIController.h"
#include "Components/CapsuleComponent.h"
#include "GMPHelper.h"
#include "ZombiePawn/ZombieMovementComponent.h"

void AZombiePawnBase::DeselectZombie()
{
	SetSelected(false);
}

void AZombiePawnBase::HandleOnPlayerInteraction(AActor* InteractedActor, FHitResult HitResult)
{
	auto* InteractedZombie = Cast<AZombiePawnBase>(InteractedActor);
	if (InteractedZombie)
	{
		SetSelected(this == InteractedZombie);
		return;
	}
	else if (bSelected)
	{
		MoveZombieToClickPosition(HitResult);
	}
}

void AZombiePawnBase::MoveZombieToClickPosition(FHitResult HitResult)
{
	AZombieAIController* ZombieController = Cast<AZombieAIController>(GetController());
	ZombieController->MoveZombieToLocation(HitResult.Location);

}

// Sets default values
AZombiePawnBase::AZombiePawnBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AZombieAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->InitCapsuleSize(34.f,88.f);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));

	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
	PlaneMesh->SetupAttachment(CapsuleComponent);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	Mesh->SetupAttachment(CapsuleComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ZombieMovementComponent = CreateDefaultSubobject<UZombieMovementComponent>(TEXT("ZombieMovementComponent"));
	ZombieMovementComponent->SetUpdatedComponent(CapsuleComponent);
	ZombieMovementComponent->UpdateNavAgent(*CapsuleComponent);

}

UPawnMovementComponent* AZombiePawnBase::GetMovementComponent() const
{
	return ZombieMovementComponent;
}

// Called when the game starts or when spawned
void AZombiePawnBase::BeginPlay()
{
	Super::BeginPlay();
	FGMPHelper::ListenWorldMessage(GetWorld(), MSGKEY("GMP.OnInteractionClicked"), this, &ThisClass::HandleOnPlayerInteraction);
	FGMPHelper::ListenWorldMessage(GetWorld(), MSGKEY("GMP.DeselectZombie"), this, &ThisClass::DeselectZombie);
	
}

void AZombiePawnBase::EndPlay(EEndPlayReason::Type Reason)
{
	FGMPHelper::UnbindMessage(MSGKEY("GMP.OnInteractionClicked"), this, &ThisClass::HandleOnPlayerInteraction);
	FGMPHelper::UnbindMessage(MSGKEY("GMP.DeselectZombie"), this, &ThisClass::DeselectZombie);
	Super::EndPlay(Reason);
}

// Called every frame
void AZombiePawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AZombiePawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AZombiePawnBase::SetSelected(bool Selected)
{
	if (PlaneMesh == nullptr)
	{
		return;
	}

	bSelected = Selected;
	PlaneMesh->SetHiddenInGame(!Selected);
}

