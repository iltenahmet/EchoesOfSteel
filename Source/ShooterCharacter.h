// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums.h"
#include "ShooterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

UCLASS()
class UNTITLEDTPS_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION(BlueprintCallable)
	void ReleaseAllButtons();

	// Damage 
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, 
							class AController * EventInstigator, AActor * DamageCauser) override; 

	// Death
	UFUNCTION(BlueprintCallable)
	void FinishDeath();
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDead() const { return bDead; }

	// Crosshair
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier(float DeltaTime);
	void StartCrosshairFireSpreadTimer();
 	FVector2d GetCrosshairLocation() const;
		
	// Ammo
	void PickupAmmo(class AAmmo* Ammo);

	// Aiming
    FORCEINLINE bool GetAiming() const { return bAiming; }

	// Items	
	FORCEINLINE int8 GetOverlappedItemCount() const {return OverlappedItemCount; }
	void IncrementOverlappedItemCount(const int8 Amount);

	// Combat
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	// Health
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetHealthPercent() const { return CurrentHealth / MaxHealth; }
	
	// Reload
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	UFUNCTION(BlueprintCallable)
	void GrabClip();
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();	

	// Melee Impact Sound
	void PlayMeleeImpactSound();

	// VFX
	void PlayBloodParticles(const FTransform Transform);

	// Movement
	UPROPERTY(EditAnywhere, Category = Movement)
	float RifleWalkSpeed = 600.f;
	UPROPERTY(EditAnywhere, Category = Movement)
	float ShotgunWalkSpeed = 450.f;
	UPROPERTY(EditAnywhere, Category = Movement)
	float RifleJumpHeight = 450.f;
	UPROPERTY(EditAnywhere, Category = Movement)
	float ShotgunJumpHeight = 300.f;
	
	// Aiming
	UPROPERTY(EditAnywhere, Category = Aiming)
	float CameraZoomedFOV = 50.f;
	UPROPERTY(EditAnywhere, Category = Aiming)
	float ZoomInterpSpeed = 18.f;

	// Trace
	UPROPERTY(EditAnywhere, Category = Trace)
	float ItemTraceDistance = 4000.f;

	// Combat
	/** Set this in Blueprints for the default Weapon class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TArray<TSubclassOf<class AWeapon>> DefaultWeaponClasses;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	int32 StartingShotgunAmmo = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	int32 StartingRifleAmmo = 70;

	// Melee Impact Sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	class USoundCue* MeleeImpactSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float MeleeImpactSoundWaitDuration = 0.2;

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* ShotgunHipFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* ShotgunReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* RifleHipFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* RifleReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* EquipMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* DeathMontage;

	// Reload
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	USceneComponent* HandSceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	FTransform ClipTransform;

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health)
	float CurrentHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health)
	float MaxHealth = 100.f;
	/* Falls longer than this duration will apply damage to the player*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health)
	double HealthyFallTime = 2.0;

	// VFX
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VFX)
	class UParticleSystem* BloodParticles;

	// Camera - Death
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float CameraBackwardOffsetAfterDeath = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float CameraZoomOutSpeedAfterDeath = 2.f;

	// Game Over Screen
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Over Screen")
	TSubclassOf	<class UUserWidget> GameOverWidgetClass;	

protected:
	// Movement and turning
 	void MoveForward(float Value);
 	void MoveRight(float Value);
 	void TurnAtRate(float Rate);
 	void LookUpAtRate(float Rate);
 	void TurnMouse(float MouseValue);
 	void LookUpMouse(float MouseValue);
 	
	void SetWalkingSpeedBasedOnWeaponType(AWeapon* Weapon);
	void SetJumpVelocityBasedOnWeaponType(AWeapon* Weapon);

	void CheckInAirTime();

	// Button press and release 
 	void AimButtonPressed();
 	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void SelectButtonPressed();
	void ReloadButtonPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();

	// Camera
 	void SetCameraFOVBasedOnAiming(float DeltaTime);
 	void MoveCameraAfterDeath(float DeltaTime);

 	// Fire
 	void Fire();
	void PlayHipFireMontage() const;
	void SetFireMontageBasedOnWeaponType(AWeapon* Weapon);
	void StartFireTimer();
	void AutoFireReset();

	// Crosshair 
    void CalculateCrosshairVelocityFactor();
	void CalculateCrosshairInAirFactor(float DeltaTime);
    void CalculateCrosshairAimFactor(float DeltaTime);
	void CalculateCrosshairFireFactor(float DeltaTime);
	void FinishCrosshairFireSpreadTimer();

	// Trace
	void TraceForItems();

	// Equip and swap weapon
	AWeapon* SpawnDefaultWeapon() const;
	void PickUpWeapon(AWeapon* Weapon);
	void EquipWeapon(AWeapon* WeaponToEquip);
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	void DropWeapon();
	void EquipWeaponFromInventory(int8 SlotIndex);

	// Ammo
	void InitializeAmmoMap();
	bool IsCarryingRightAmmoType();
	void ExtractExtraAmmoIntoAmmoMap(AWeapon* Weapon);

	// Melee Impact Sound
	FORCEINLINE void MeleeImpactSoundTimerReset() { bMeleeImpactTimerInProgress = false;}

	// Death
	void Die();

	// Camera	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraSpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FollowCamera;
 
	// Camera - Aiming
	float CameraDefaultFOV = 0.f; // Will be set in BeginPlay
	float CameraCurrentFOV = 0.f; // Will be set in BeginPlay

	// Camera - After Death
	float TargetCameraSpringArmLengthAfterDeath;

	// Turn and LookUp
	float HipTurnRate = 45.f;
	float HipLookUpRate = 45.f;
	float AimingTurnRate = 20.f;
	float AimingLookUpRate = 20.f;
	
	// Input
	bool bFireButtonPressed = false;
    bool bAiming = false;

    // Animations
	UAnimMontage* HipFireMontage;

	// In Air Time 
	double InAirTime;

	// FireTimer
	FTimerHandle AutoFireTimer;

	//Crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair)
	float CrosshairUpwardOffset = 25.f; //Raises crosshair location upward
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair)
	float CrosshairSpreadMax = 20.f;
	FTimerHandle CrosshairFireSpreadTimer;
	bool bCrosshairSpreadOnFire = false;
	float CrosshairSpreadDurationOnFire = 0.05f;

	// Crosshair  Calculations
	float CrosshairFireFactor = 0.f;	
	float CrosshairVelocityFactor = 0.f;
	float CrosshairInAirFactor = 0.f;
	float CrosshairAimFactor = 0.f;

	// Line Trace	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trace)
	class AItem* TraceHitItem;
	bool bShouldTraceForItems = false;
	int8 OverlappedItemCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trace)
	AItem* TraceHitItemLastFrame;

	// Items
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	TMap<EAmmoType, int32> AmmoMap;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	TArray<AItem*> Inventory;
	const int8 INVENTORY_SIZE = 3;
	/* Delegate for sending slot information to the inventory bar when equipping */
	UPROPERTY(BlueprintAssignable, Category = Delegates)
	FEquipItemDelegate EquipItemDelegate;

	// Melee Impact Sound
	FTimerHandle MeleeImpactSoundTimer;
	bool bMeleeImpactTimerInProgress = false;

	// Death
	bool bDead = false;
};
