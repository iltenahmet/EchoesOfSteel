// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemyShooter.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API AEnemyShooter : public AEnemy
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable)
	void Fire();

	/** Set this in Blueprints for the default Weapon class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TSubclassOf<class AWeapon> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MissChance = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ViewDistance = 10000.0f;

protected:
	void Die() override;

	AWeapon* SpawnDefaultWeapon() const;
	void EquipWeapon(AWeapon* WeaponToEquip);

	FVector LastTargetPosition;
};
