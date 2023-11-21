// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class UNTITLEDTPS_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FHitResult& Hit) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	void ShowHitNumber(int32 Damage, FVector HitLocation);

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	// FX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
	UParticleSystem* GetHitParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
	class USoundCue* GetHitSound;

	//Health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health)
	float MaxHealth = 100.f;

	// HeadShot
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Head Shot")
	FString HeadBoneName;

	// Animation - Hit
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* HitMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	float HitReactTimeMin = 0.1f;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	float HitReactTimeMax = 0.5f;	

	// Animation - Death
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* DeathMontage;

	// Hit Numbers
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	TSubclassOf<class UHitNumberWidget> HitNumberWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	float HitNumberDestroyTime = 1.f;

	//AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	class UBehaviorTree* BehaviorTree; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (MakeEditWidget = "true"))
	FVector PatrolPoint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (MakeEditWidget = "true"))
	FVector PatrolPoint2;

	// Death
	/* Sets how long we'll wait before enemy gets destroyed after it has died.**/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Death)
	float EnemyDestroyWaitTime = 2.f;	

protected:
	virtual void Die();

	void PlayHitMontage(const FName SectionName,const float PlayRate = 1.f);

	void ResetHitReactTimer();

	void Destroy();

	// AI
	class AEnemyAIController* AIController;

	// Hit Numbers
	void StoreHitNumber(UUserWidget* Widget, const FVector Location);
	UFUNCTION()
	void DestroyHitNumber(UUserWidget* Widget);
	void UpdateHitNumbers();

	// Player
	class AShooterCharacter* ShooterCharacter;

	// Animation 
	FTimerHandle HitReactTimer;
	bool bHitReactTimerInProgress = false;

	// Hit Numbers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TMap<UUserWidget*, FVector> HitNumberWidgets;

	// Death
	bool bDead = false;
	FTimerHandle EnemyDestroyTimer;	
};
