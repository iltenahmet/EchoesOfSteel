// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyShooterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (!EnemyCharacter) EnemyCharacter = Cast<AEnemy>(TryGetPawnOwner());
 	if (!EnemyCharacter) return;
	
	FVector Velocity = EnemyCharacter->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();

	bIsFalling = EnemyCharacter->GetCharacterMovement()->IsFalling();

	const float Acceleration = EnemyCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size();
	bIsAccelerating = Acceleration > 0.f ? true : false;
}

void UEnemyShooterAnimInstance::NativeInitializeAnimation() 
{
	EnemyCharacter = Cast<AEnemy>(TryGetPawnOwner());
}
