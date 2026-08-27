// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Storages/StorageCupboardData.h"
#include "StorageComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UStorageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStorageComponent();

	UFUNCTION(BlueprintCallable)
	bool AddItemToCupboard(const FStorageCupboardProductsData& Product);

	UFUNCTION(BlueprintCallable)
	bool PopItemFromCupboard(const FString& ProductId);

	UFUNCTION(BlueprintCallable)
	void SaveItemsInCupboard();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ShelfCapacity = 3;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	UPROPERTY()
	FStorageCupboardData Data;
};
