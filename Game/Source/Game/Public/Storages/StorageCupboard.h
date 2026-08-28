// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "StorageCupboard.generated.h"

class UStaticMeshComponent;
class UStorageComponent;

UCLASS()
class GAME_API AStorageCupboard : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStorageCupboard();

	void OnInteraction_Implementation() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MainMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UStaticMeshComponent>> ShelvesComponents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStorageComponent> StorageComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ShelvesNum = 4;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
