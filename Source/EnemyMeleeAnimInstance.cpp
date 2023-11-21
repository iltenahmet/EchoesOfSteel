// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMeleeAnimInstance.h"
#include "EnemyMelee.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyMeleeAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (!EnemyCharacter) EnemyCharacter = Cast<AEnemyMelee>(TryGetPawnOwner());
 	if (!EnemyCharacter) return;
	
	FVector Velocity = EnemyCharacter->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();
}

void UEnemyMeleeAnimInstance::NativeInitializeAnimation() 
{
	EnemyCharacter = Cast<AEnemyMelee>(TryGetPawnOwner());
}
