// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "EnemyAIController.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "HitNumberWidget.h"
#include "Macros.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	CurrentHealth = MaxHealth;
	
	AIController = Cast<AEnemyAIController>(GetController());

	FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	if (AIController)
	{
		AIController->BlackboardComponent->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		AIController->BlackboardComponent->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);

		AIController->RunBehaviorTree(BehaviorTree);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHitNumbers();

	if (ShooterCharacter && ShooterCharacter->IsDead())
	{
		if (AIController) AIController->BlackboardComponent->SetValueAsBool(TEXT("PlayerDead"), true);
	}
}

void AEnemy::GetHit_Implementation(const FHitResult& Hit)
{
	if (bDead) return;

	if (GetHitSound)
	{
		UGameplayStatics::PlaySound2D(this, GetHitSound);
	}
	if (GetHitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, GetHitParticles, Hit.Location);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bDead) return DamageAmount;

	if (CurrentHealth - DamageAmount <= 0)
	{
		CurrentHealth = 0;	
		Die();
	}
	else
	{
		CurrentHealth -= DamageAmount;
	}

	if (!AIController) return DamageAmount;
	AShooterCharacter* LocalShooterCharacter = Cast<AShooterCharacter>(DamageCauser->GetOwner());
	if (!LocalShooterCharacter) return DamageAmount;
	ShooterCharacter = LocalShooterCharacter;
	// Setting this variable will aggro the enemy towards the player
	AIController->BlackboardComponent->SetValueAsObject(TEXT("Target"), ShooterCharacter);

	return DamageAmount;
}

void AEnemy::Die()
{
	if (bDead) return;
	bDead = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();		
	if (!AnimInstance) return;

	if (!DeathMontage)
	{
		LOG_FSTRING_ON_SCREEN("DeathMontage is not set on enemy.")
		return;		
	}

	AnimInstance->Montage_Play(DeathMontage);
	
	if (AIController)
	{
		AIController->BlackboardComponent->SetValueAsBool(FName("Dead"), true);
	}
}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	GetWorldTimerManager().SetTimer(EnemyDestroyTimer, this, &AEnemy::Destroy, EnemyDestroyWaitTime);
}

void AEnemy::PlayHitMontage(const FName SectionName,const float PlayRate)
{
	if (bHitReactTimerInProgress || bDead) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();		
	if (!AnimInstance) return;
	if (!HitMontage)
	{
		LOG_FSTRING_ON_SCREEN("HitMontage is not set on enemy.")
		return;
	}

	AnimInstance->Montage_Play(HitMontage, PlayRate);
	AnimInstance->Montage_JumpToSection(SectionName, HitMontage);

	bHitReactTimerInProgress = true;
	const float HitReactTime = FMath::FRandRange(HitReactTimeMin, HitReactTimeMax);
	GetWorldTimerManager().SetTimer(HitReactTimer, this, &AEnemy::ResetHitReactTimer, HitReactTime);
}

void AEnemy::ResetHitReactTimer()
{
	bHitReactTimerInProgress = false;
}

void AEnemy::ShowHitNumber(int32 Damage, FVector HitLocation)
{
	if (bDead) return;
	if (!HitNumberWidgetClass)
	{
		LOG_FSTRING_ON_SCREEN(TEXT("HitNumberWidgetClass is not set on the enemy!"))	
		return;
	}

	UHitNumberWidget* Widget = CreateWidget<UHitNumberWidget>(GetWorld(), HitNumberWidgetClass);
	if (!Widget) return;

	Widget->AddToViewport();

	FVector2d ScreenPosition; 
	UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), HitLocation, ScreenPosition);
	Widget->SetPositionInViewport(ScreenPosition);

	StoreHitNumber(Widget, HitLocation);
	Widget->Damage = Damage;
	Widget->DamageTier = Damage/10 + 1; // 0-9 -> Tier 1, 10-19 -> Tier 2 and so on.
	Widget->PlayAnimation(Widget->SlideAndFadeAnimation);
}

void AEnemy::StoreHitNumber(UUserWidget* Widget, const FVector Location)
{
	HitNumberWidgets.Add(Widget, Location);	

	FTimerHandle HitTimerHandle;
	FTimerDelegate HitTimerDelegate;
	HitTimerDelegate.BindUFunction(this, FName("DestroyHitNumber"), Widget);
	GetWorldTimerManager().SetTimer(HitTimerHandle, HitTimerDelegate, HitNumberDestroyTime, false);
}

void AEnemy::DestroyHitNumber(UUserWidget* Widget)
{
	HitNumberWidgets.Remove(Widget);
	Widget->RemoveFromParent();
}

void AEnemy::UpdateHitNumbers()
{
	for (auto& HitNumberPair : HitNumberWidgets)
	{
		UUserWidget* Widget = HitNumberPair.Key;
		const FVector Location = HitNumberPair.Value;

		FVector2d ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);
		Widget->SetPositionInViewport(ScreenPosition);
	}
}

void AEnemy::Destroy()
{
	Super::Destroy();
}
