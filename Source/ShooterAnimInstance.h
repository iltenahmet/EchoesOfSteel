// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enums.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties (float DeltaTime);

	virtual void NativeInitializeAnimation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float StrafingOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float LastStrafingOffset = 0.f;

protected:
	void TurnInPlace();

	void LeanWhileRunning(float DeltaTime);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	class AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float Speed = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsFalling = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsAccelerating = false;

	/** Turn In Place (TIP) - Only updated when standing still and not in air */
	float TIPCharacterYaw = 0.f;
	float TIPCharacterYawLastFrame = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	float RootYawOffset = 0.f;

	float RotationCurve;
	float RotationCurveLastFrame;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aiming)
	bool bAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aiming)
	float Pitch = 0.f;

	/** Used to prevent aim offset when reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aiming)
	bool bReloading = 0.f;

	FRotator CharacterRotation = FRotator(0.f);
	FRotator CharacterRotationLastFrame = FRotator(0.f);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean)
	float YawDelta = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	EWeaponType WeaponType = EWeaponType::EWT_Shotgun;

};

