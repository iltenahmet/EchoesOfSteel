// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyShooter.h"
#include "EnemyAIController.h"
#include "ShooterCharacter.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "Engine/SkeletalMeshSocket.h"
#include "Macros.h"

void AEnemyShooter::BeginPlay()
{
	Super::BeginPlay();
	
	if(AWeapon* DefaultWeapon = SpawnDefaultWeapon())
	{
		EquipWeapon(DefaultWeapon);
	}
}

void AEnemyShooter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (AIController)
	{
		APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);

		if (AIController->LineOfSightTo(Player) 
			&& GetDistanceTo(Player) <= ViewDistance)
		{
			ShooterCharacter = Cast<AShooterCharacter>(Player);
			if (ShooterCharacter)
			{
				AIController->BlackboardComponent->SetValueAsObject(TEXT("Target"), ShooterCharacter);
				LastTargetPosition = ShooterCharacter->GetActorLocation();
				AIController->BlackboardComponent->SetValueAsVector(TEXT("LastTargetPosition"), LastTargetPosition);
			}
		}
		else 
		{
			AIController->BlackboardComponent->ClearValue(TEXT("Target"));
		}
	}
}

void AEnemyShooter::GetHit_Implementation(const FHitResult &Hit)
{
	if (bDead) return;

	Super::GetHit_Implementation(Hit);		
	PlayHitMontage(FName("HitFront"));
}

AWeapon* AEnemyShooter::SpawnDefaultWeapon() const
{
	if (!DefaultWeaponClass) return nullptr;
	return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
}

void AEnemyShooter::Fire()
{
	if (!EquippedWeapon || bDead) return;

	float RandomValue =	FMath::FRandRange(0.f, 1.f);
	bool bMiss;
	if (RandomValue <= MissChance)
	{
		bMiss = true;
	}
	else
	{
		bMiss = false;
	}

	EquippedWeapon->PullTrigger(bMiss);
	EquippedWeapon->ReloadAmmo(1);
}

void AEnemyShooter::Die()
{
	Super::Die();
}

void AEnemyShooter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!WeaponToEquip) return;

	WeaponToEquip->SetOwner(this);
	WeaponToEquip->UpdateItemStateAndProperties(EItemState::EIS_Equipped);

	FName GunSocketName = WeaponToEquip->GunSocketName;
	if(const USkeletalMeshSocket* GunSocket = GetMesh()->GetSocketByName(GunSocketName))
	{
		GunSocket->AttachActor(WeaponToEquip, GetMesh());
	}

	EquippedWeapon = WeaponToEquip;
}
