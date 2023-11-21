// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemyMelee.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API AEnemyMelee : public AEnemy
{
	GENERATED_BODY()

public:
	AEnemyMelee();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetHit_Implementation(const FHitResult& Hit) override;

	// Collision
	UFUNCTION(BlueprintCallable)
	void ActivateLeftWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateLeftWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void ActivateRightWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateRightWeaponCollision();

	// Attack 
	void HitPlayer(AActor* Victim, const FHitResult& HitResult);
	UFUNCTION(BlueprintPure)
	FName GetAttackMontageSectionName() const;
	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName SectionName, const float PlayRate = 1.f);

	// Stun
	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Value);

	// AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	class USphereComponent* AgroSphere; 

	// Stun
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float StunChance = 0.5f;

	// Attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	class USphereComponent* AttackRangeSphere; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)	
	class UAnimMontage* AttackMontage; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)	
	class UBoxComponent* LeftWeaponCollision;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)	
	UBoxComponent* RightWeaponCollision;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)	
	float BaseDamage = 10.f;

protected:
	UFUNCTION()
	void OnAgroSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackRangeSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackRangeSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void OnLeftWeaponOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnRightWeaponOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/** True when the hit animation is playing. */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bStunned = false;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bInAttackRange = false;	
};
