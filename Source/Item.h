// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Item.generated.h"

UCLASS()
class UNTITLEDTPS_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetPickupWidgetVisibility(bool Visibility);

	void UpdateItemStateAndProperties(EItemState ItemState);
	virtual void UpdateItemProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, Category = "Item Properties")
	class UCapsuleComponent* CollisionCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	FString ItemName = "DefaultItemName";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	int32 ItemCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	int32 ItemPower = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	class USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	USoundCue* EquipSound;

	/* Should be set from blueprints in Event BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FSlateBrush IconBackground;

	/* Should be set from blueprints in Event BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FSlateBrush ItemIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 InventorySlotIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool bInventoryIsFull = false;

protected:
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	void SetActiveStarsBasedOnPower();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties")
	TArray<bool> ActiveStars;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EItemState ItemState = EItemState::EIS_Pickup;
};
