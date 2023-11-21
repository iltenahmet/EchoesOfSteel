// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "ShooterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "BulletHitInterface.h"
#include "Enemy.h"
#include "EnemyShooter.h"
#include "Macros.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	MuzzleSocket = ItemMesh->GetSocketByName("Muzzle_01");
}

void AWeapon::ThrowWeapon(FVector ImpulseDirection, const float ImpulsePower)
{
	const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
	ItemMesh->DetachFromComponent(DetachmentTransformRules);
	UpdateItemStateAndProperties(EItemState::EIS_Falling);
	SetOwner(nullptr);

	const float RandomRotation = FMath::FRandRange(10.f, 30.f);
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection.Normalize();	
	ImpulseDirection *= ImpulsePower;
	ItemMesh->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::ReloadAmmo(const int32 Amount)
{
	int32 Total = AmmoCount + Amount;
	if(Total >= MagazineCapacity)
	{
		AmmoCount = MagazineCapacity;	
	} 
	else
	{
		AmmoCount = Total;
	}
}

bool AWeapon::PullTrigger(bool bMiss)
{
	if (AmmoCount <= 0) return false;

	AmmoCount--;
	SpawnMuzzleFlash();
	PlayFireSound();

	int32 LineTraceCount = 1;
	if (WeaponType == EWeaponType::EWT_Shotgun) LineTraceCount = 6;

	for (int8 i = 0; i < LineTraceCount; i++)
	{
		bool bRandomVariation = (i == 0) ? false : true;

		FHitResult FireHit;
		const bool bSuccess = GetFireHitResult(FireHit, bMiss, bRandomVariation);
		if (!bSuccess) continue;

		if (IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(FireHit.GetActor()))
		{
			BulletHitInterface->GetHit_Implementation(FireHit);
		}
		else
		{
			PlayFireImpactParticles(FireHit.Location, FireHit.bBlockingHit);
		}

		if(AEnemy* Enemy = Cast<AEnemy>(FireHit.GetActor()))
		{
			ApplyDamageToEnemy(Enemy, FireHit);
		}
		else if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(FireHit.GetActor()))
		{
			ApplyDamageToPlayer(ShooterCharacter, FireHit);
		}
	}

	return true;
}

bool AWeapon::GetFireHitResult(FHitResult &OutHit, bool bMiss, bool bRandomVariation) 
{	
	if (!MuzzleSocket)
	{
		LOG_FSTRING_ON_SCREEN(TEXT("The socket 'Muzzle_01' must be set on the equipped weapon."))
		return false;
	}

	if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner())) 
	{
		TraceFromPlayer(ShooterCharacter, OutHit, bRandomVariation);	
	}	
	else if (AEnemyShooter* Enemy = Cast<AEnemyShooter>(GetOwner())) 
	{
		TraceFromEnemy(Enemy, OutHit, bMiss, bRandomVariation);
	}

	return true;
}

void AWeapon::TraceFromPlayer(AShooterCharacter* ShooterCharacter, FHitResult &OutHit, bool bRandomVariation)
{
	FVector2d CrosshairLocation = ShooterCharacter->GetCrosshairLocation();

	// project crosshair onto the world
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bDeprojectSuccess = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);
	
	//Trace from crosshair world location outward
	const FVector Start = CrosshairWorldPosition;
	FVector End = Start + CrosshairWorldDirection * Range;

	if (bRandomVariation)
	{
		End = RandomVariationToLineTraceEnd(End, CrosshairWorldDirection);
	}

	if (bDeprojectSuccess)
	{
		GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility);
	}
}

void AWeapon::TraceFromEnemy(AEnemyShooter* Enemy, FHitResult &OutHit, bool bMiss, bool bRandomVariation)
{
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Enemy);	

		// Line trace from enemy's viewpoint
		FVector Start;
		FRotator Rotation;
		Enemy->GetController()->GetPlayerViewPoint(Start, Rotation);
		FVector PawnTraceEnd = Start + Rotation.Vector() * Range;
		FHitResult PawnTraceHit;	
		GetWorld()->LineTraceSingleByChannel(PawnTraceHit, Start, PawnTraceEnd, ECC_Visibility, CollisionParams);

		FVector	PawnTraceHitPoint = PawnTraceHit.bBlockingHit ? PawnTraceHit.Location : PawnTraceEnd;

		// line trace from the gun
		const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(ItemMesh);
		FVector WeaponTraceStart = SocketTransform.GetLocation();
		FVector WeaponTraceDirection = (PawnTraceHitPoint - WeaponTraceStart).GetSafeNormal();
		FVector WeaponTraceEnd = PawnTraceHitPoint + WeaponTraceDirection * 1000;

		if (bMiss || bRandomVariation)
		{
			WeaponTraceEnd = RandomVariationToLineTraceEnd(WeaponTraceEnd, WeaponTraceDirection);
		}

		GetWorld()->LineTraceSingleByChannel(OutHit, WeaponTraceStart, WeaponTraceEnd, ECC_Visibility, CollisionParams);
}

FVector AWeapon::RandomVariationToLineTraceEnd(FVector Target, FVector TraceDirection)
{
	// Making sure our random offset is to the right or to the left of our target, so we miss it.
	FVector TargetRight = FVector::CrossProduct(TraceDirection, FVector::UpVector);
    float RandomOffset = FMath::RandRange(100, 150);
    if (FMath::RandBool()) RandomOffset = -RandomOffset;
    FVector RandomLeftOrRightVector = TargetRight * RandomOffset;

    float RandomOffsetUp = FMath::RandRange(-50, 150);
    FVector RandomUpVector = FVector::UpVector * RandomOffsetUp;

	return Target + RandomUpVector + RandomLeftOrRightVector;
}

void AWeapon::SpawnMuzzleFlash() const
{
	if (!MuzzleSocket) return;
	
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(ItemMesh);
	if (MuzzleFlash) // muzzle flash should be set up in the editor
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
	}
}

void AWeapon::PlayFireSound() const
{
	if (FireSoundCue)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FireSoundCue);
	}
}

void AWeapon::PlayFireImpactParticles(const FVector& FireHitPoint, bool bFireHit) const
{
	if (bFireHit && ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHitPoint);
	}
}

void AWeapon::StopFalling()
{
	bFalling = false;
	UpdateItemStateAndProperties(EItemState::EIS_Pickup);
}
	
void AWeapon::ApplyDamageToEnemy(AEnemy* Enemy, const FHitResult& Hit)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());
	if (!ShooterCharacter) return;

	float Damage = (*Hit.BoneName.ToString() == Enemy->HeadBoneName) ? HeadShotDamage : BodyDamage;
	float RandomVariation = FMath::FRandRange(-DamageVariation, DamageVariation);
	Damage += RandomVariation;
	
	Enemy->ShowHitNumber(Damage, Hit.Location);
	UGameplayStatics::ApplyDamage(Enemy, Damage, ShooterCharacter->GetController(), this, UDamageType::StaticClass());
}

void AWeapon::ApplyDamageToPlayer(AShooterCharacter* ShooterCharacter, const FHitResult& Hit)
{
	AEnemy* Enemy = Cast<AEnemy>(GetOwner());
	if (!Enemy) return;

	UGameplayStatics::ApplyDamage(ShooterCharacter, BodyDamage, Enemy->GetController(), this, UDamageType::StaticClass());
}
