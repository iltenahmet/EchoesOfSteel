// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyMeleeAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API UEnemyMeleeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	class AEnemyMelee* EnemyCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float Speed = 0;
	
};
