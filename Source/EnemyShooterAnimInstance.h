// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy.h"
#include "EnemyShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API UEnemyShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties (float DeltaTime);

	virtual void NativeInitializeAnimation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	class AEnemy* EnemyCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float Speed = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsFalling = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsAccelerating = false;

};
