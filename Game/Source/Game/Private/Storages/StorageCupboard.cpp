// Fill out your copyright notice in the Description page of Project Settings.


#include "Storages/StorageCupboard.h"
#include "Components/StaticMeshComponent.h"
#include "Components/StorageComponent.h"
#include "CafeDefinitions.h"
#include "GMPHelper.h"

// Sets default values
AStorageCupboard::AStorageCupboard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	MainMesh->SetupAttachment(GetRootComponent());

	StorageComponent = CreateDefaultSubobject<UStorageComponent>(TEXT("StorageComponent"));

	ShelvesComponents.Empty();

	if (StorageComponent)
	{
		for (int i = 0; i < StorageComponent->GetShelvesNum(); i++)
		{
			auto Shelf = CreateDefaultSubobject<UStaticMeshComponent>(*FString("Shelf_" + FString::FromInt(i)));
			Shelf->SetupAttachment(MainMesh);
			ShelvesComponents.Add(Shelf);
		}
	}

}

void AStorageCupboard::OnInteraction_Implementation()
{
	FGMPHelper::SendWorldMessage(GetWorld(), MSGKEY("GMP.OnInteractionClicked"), this);
}

EInteractionType AStorageCupboard::GetInteractionType_Implementation()
{
	return EInteractionType::StorageCupboard;
}

// Called when the game starts or when spawned
void AStorageCupboard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStorageCupboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

