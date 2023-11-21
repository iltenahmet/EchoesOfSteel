// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (!ShooterCharacter) ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
 	if (!ShooterCharacter) return;
	
	bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;

	FVector Velocity = ShooterCharacter->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();

	bIsFalling = ShooterCharacter->GetCharacterMovement()->IsFalling();

	const float Acceleration = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size();
	bIsAccelerating = Acceleration > 0.f ? true : false;

	const FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

	//normalized difference between AimRotation and MovementRotation
	StrafingOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	if (ShooterCharacter->GetVelocity().Size() > 0.f) LastStrafingOffset = StrafingOffset;

	bAiming = ShooterCharacter->GetAiming();

	if (ShooterCharacter->EquippedWeapon)
	{
		WeaponType = ShooterCharacter->EquippedWeapon->WeaponType;
	}	

	TurnInPlace();
	LeanWhileRunning(DeltaTime);
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (!ShooterCharacter) return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	if (Speed > 0.f)
	{
		RootYawOffset = 0;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = 0.f;
		RotationCurve = 0.f;
		RotationCurveLastFrame = 0.f;
		return;
	}

	TIPCharacterYawLastFrame = TIPCharacterYaw;
	TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
	const float TIPYawDelta  = TIPCharacterYaw - TIPCharacterYawLastFrame;

	// Update RootYawOffset and clamp to [-180, 180]
	RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

	const bool Turning = GetCurveValue(TEXT("Turning")) > 0 ? true : false;
	if (Turning)
	{
		RotationCurveLastFrame = RotationCurve;
		RotationCurve = GetCurveValue(TEXT("Rotation"));
		const float DeltaRotation = RotationCurve - RotationCurveLastFrame;

		// If RootYawOffset > 0, we're turning left. Otherwise we're turning right
		RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation; 

		const float ABSRootYawOffset = FMath::Abs(RootYawOffset);
		if (ABSRootYawOffset > 90)
		{
			const float YawExcess = ABSRootYawOffset - 90.f;
			RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
		}
	}
}

void UShooterAnimInstance::LeanWhileRunning(float DeltaTime)
{
	if (!ShooterCharacter) return;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	
	const float Target = (Delta.Yaw) / DeltaTime;
	const float Interp = FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f);
	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);
}
