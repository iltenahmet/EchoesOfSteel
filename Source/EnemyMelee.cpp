// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyMelee.h"
#include "EnemyAIController.h"
#include "ShooterCharacter.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Macros.h"
#include <ostream>

AEnemyMelee::AEnemyMelee()
{
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
	AttackRangeSphere->SetupAttachment(GetRootComponent());

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollision"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponBone"));
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollision"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));
}

void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMelee::OnAgroSphereOverlap);
	AttackRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMelee::OnAttackRangeSphereOverlap);
	AttackRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyMelee::OnAttackRangeSphereEndOverlap);

	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMelee::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMelee::OnRightWeaponOverlap);

	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AEnemyMelee::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	if (AIController && ShooterCharacter)
	{
		bool bShooterInMotion = ShooterCharacter->GetVelocity().Size() > 0 ? true : false;
		AIController->BlackboardComponent->SetValueAsBool(TEXT("TargetInMotion"), bShooterInMotion);
	}	
}

void AEnemyMelee::GetHit_Implementation(const FHitResult& Hit)
{
	if (bDead) return;

	Super::GetHit_Implementation(Hit);

	const float RandomStunValue = FMath::FRandRange(0.f, 1.f);
	if (RandomStunValue <= StunChance)
	{
		PlayHitMontage(FName("HitFront"));
		SetStunned(true);
	}
}

void AEnemyMelee::ActivateLeftWeaponCollision()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyMelee::DeactivateLeftWeaponCollision()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyMelee::ActivateRightWeaponCollision()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyMelee::DeactivateRightWeaponCollision()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyMelee::SetStunned(bool Value)
{
	bStunned = Value;
	if (AIController) AIController->BlackboardComponent->SetValueAsBool(TEXT("Stunned"), bStunned);
}

void AEnemyMelee::HitPlayer(AActor* Victim, const FHitResult& HitResult)
{
	if (!Victim || !AIController) return;
	AShooterCharacter* LocalShooterCharacter = Cast<AShooterCharacter>(Victim);
	if (!LocalShooterCharacter) return;

	// Apply Damage
	UGameplayStatics::ApplyDamage(
		LocalShooterCharacter, 
		BaseDamage, 
		AIController, 
		this, 
		UDamageType::StaticClass());

	// Play Impact Sound
	LocalShooterCharacter->PlayMeleeImpactSound();

	//Play Blood Particles
	if (!HitResult.bBlockingHit) return;
	FTransform Transform;
	Transform.SetLocation(HitResult.Location);
	Transform.SetRotation(UKismetMathLibrary::RandomRotator().Quaternion());
	LocalShooterCharacter->PlayBloodParticles(Transform);
}

void AEnemyMelee::OnAgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                            bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || bDead || !AIController) return;

	AShooterCharacter* LocalShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (!LocalShooterCharacter) return;

	ShooterCharacter = LocalShooterCharacter;
	AIController->BlackboardComponent->SetValueAsObject(TEXT("Target"), ShooterCharacter);
}

void AEnemyMelee::OnAttackRangeSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                            bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || bDead) return;
	
	AShooterCharacter* LocalShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (!LocalShooterCharacter) return;

	if (!AIController) return;

	bInAttackRange = true;
	AIController->BlackboardComponent->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
}
void AEnemyMelee::OnAttackRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AIController)
	{
		bInAttackRange = false;
		AIController->BlackboardComponent->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
	}
}

void AEnemyMelee::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                            bool bFromSweep, const FHitResult& SweepResult)
{
	if (bDead) return;

	FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    FHitResult HitResult; // SweepResult is not populated by default so we create our own HitResult
    GetWorld()->LineTraceSingleByChannel(
    	HitResult, 
    	LeftWeaponCollision->GetComponentLocation(), 
    	OtherActor->GetActorLocation(), 
    	ECC_HIT, 
    	Params);

	HitPlayer(OtherActor, HitResult);
}

void AEnemyMelee::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                            bool bFromSweep, const FHitResult& SweepResult)
{
	if (bDead) return;

	FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    FHitResult HitResult; // SweepResult is not populated by default so we create our own HitResult
    GetWorld()->LineTraceSingleByChannel(
    	HitResult, 
    	RightWeaponCollision->GetComponentLocation(), 
    	OtherActor->GetActorLocation(), 
    	ECC_HIT, 
    	Params);

	HitPlayer(OtherActor, HitResult);
}

void AEnemyMelee::PlayAttackMontage(FName SectionName, const float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (!AttackMontage) LOG_FSTRING_ON_SCREEN(TEXT("AttackMontage is not set on EnemyMelee."))

	AnimInstance->Montage_Play(AttackMontage, PlayRate);
	AnimInstance->Montage_JumpToSection(SectionName);
}

FName AEnemyMelee::GetAttackMontageSectionName() const
{
	FString SectionStr = "Attack_";
	const uint8 SectionNum = FMath::RandRange(1, 6);
	char SectionLetter =  'A' + (SectionNum - 1); // 1 corresponds to A, 2 to B, and so on
	SectionStr += SectionLetter;
	return FName(SectionStr);
}
