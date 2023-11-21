// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Enums.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

protected:
	virtual void BeginPlay() override;

public:
	// Throw Weapon
	/** Adds an impulse to the weapon */
	UFUNCTION(BlueprintCallable)
	void ThrowWeapon(FVector ImpulseDirection, const float ImpulsePower);

	// Pull Trigger 
	bool PullTrigger(bool bMiss);

	// Reload
	void ReloadAmmo(const int32 Amount);

	// Weapon Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	int32 MagazineCapacity = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	EWeaponType WeaponType = EWeaponType::EWT_Shotgun;

	// FX
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
	USoundCue* FireSoundCue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
	UParticleSystem* MuzzleFlash;

	// Ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 AmmoCount = 0; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	EAmmoType AmmoType = EAmmoType::EAT_Shotgun;
	/* Should be set from blueprints in Event BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Ammo");	
	FSlateBrush AmmoIcon;
	
	// Reload
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Reload)
	FName MagazineBoneName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reload)
	bool bMagazineMoving;

	// Combat
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float AutoFireWaitDuration = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float BodyDamage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float HeadShotDamage = 50.f;
	/** Damage will vary +- DamageVariation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float DamageVariation = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	int32 Range = 5000;

	/* After throwing the weapon, how long should wait before entering picked up state*/
	UPROPERTY(EditAnywhere)	
	float ThrowWeaponTime = 2.f;

	/* Name of the gun socket that the weapon will attach to on the character skeletal mesh*/  
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	FName GunSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	const USkeletalMeshSocket* MuzzleSocket; 

protected:
	// Pull Trigger 
	bool GetFireHitResult(FHitResult& OutHitResult, bool bMiss, bool bRandomVariation);
	void ApplyDamageToEnemy(class AEnemy* Enemy, const FHitResult& Hit);
	void ApplyDamageToPlayer(class AShooterCharacter* ShooterCharacter, const FHitResult& Hit);
	void TraceFromPlayer(AShooterCharacter* ShooterCharacter, FHitResult &OutHit, bool bRandomVariation);
	void TraceFromEnemy(class AEnemyShooter* Enemy, FHitResult &OutHit, bool bMiss, bool bRandomVariation);
	FVector RandomVariationToLineTraceEnd(FVector Target, FVector TraceDirection);

	// Fire FX	
	void SpawnMuzzleFlash() const;
	void PlayFireImpactParticles(const FVector& FireHitPoint, bool bFireHit) const;
	void PlayFireSound() const;
	
	// Throw Weapon
	void StopFalling();
	
	// Throw Weapon
	FTimerHandle ThrowWeaponTimer;
	bool bFalling = false;

	// Fire FX
	class UAnimMontage* HipFireMontage;
};
