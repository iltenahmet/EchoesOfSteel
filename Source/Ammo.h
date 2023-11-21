// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Enums.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API AAmmo : public AItem
{
	GENERATED_BODY()

public:
	AAmmo();

	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;

	virtual void UpdateItemProperties() override;
	
public:	
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	EAmmoType AmmoType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	UTexture2D* AmmoIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	USphereComponent* AmmoCollisionSphere;
		
protected:
	UFUNCTION()
	void OnAmmoSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo)
	UStaticMeshComponent* AmmoMesh;
	
};
