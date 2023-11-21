// Fill out your copyright notice in the Description page of Project Settings. 

#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "ShooterGameInstance.h"
#include "LevelFinishItem.h"
#include "Macros.h"
#include "Item.h"
#include "Weapon.h"
#include "Ammo.h"
#include "Enemy.h"
#include "EnemyAIController.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystem.h"
#include "BehaviorTree/BlackboardComponent.h" 

// Sets default values
AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 270.f;
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->SocketOffset = FVector(0.f, 90.f, 90.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // we want camera to follow the camera boom

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true; // yaw rotation of controller affects character aiming

	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RightHandSceneComponent"));

	GetCharacterMovement()->bOrientRotationToMovement = false; // character rotates according to movement direction
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (FollowCamera)
	{
		CameraDefaultFOV = FollowCamera->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	if(DefaultWeaponClasses.Num() != 0)
	{
		for (int8 i = 0; i < DefaultWeaponClasses.Num(); i++)
		{
			AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClasses[i]);
			if (!Weapon) break;

			PickUpWeapon(Weapon);

			if (i == 0)
			{
				EquipWeapon(Weapon);
				FinishEquipping();
			}
		}
	}

	InitializeAmmoMap();
	
	GetMesh()->SetCollisionResponseToChannel(ECC_HIT, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	TargetCameraSpringArmLengthAfterDeath = CameraSpringArm->TargetArmLength + CameraBackwardOffsetAfterDeath;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bDead)
	{
		MoveCameraAfterDeath(DeltaTime);	
		return;
	}

	SetCameraFOVBasedOnAiming(DeltaTime);
	TraceForItems();	

	CheckInAirTime();
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("TurnMouse", this, &AShooterCharacter::TurnMouse);
	PlayerInputComponent->BindAxis("LookUpMouse", this, &AShooterCharacter::LookUpMouse);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimButton", IE_Pressed, this, &AShooterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("AimButton", IE_Released, this, &AShooterCharacter::AimButtonReleased);
	
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);
	
	PlayerInputComponent->BindAction("OneKey", IE_Pressed, this, &AShooterCharacter::OneKeyPressed);
	PlayerInputComponent->BindAction("TwoKey", IE_Pressed, this, &AShooterCharacter::TwoKeyPressed);
	PlayerInputComponent->BindAction("ThreeKey", IE_Pressed, this, &AShooterCharacter::ThreeKeyPressed);
}

void AShooterCharacter::ReleaseAllButtons()
{
	FireButtonReleased();
	AimButtonReleased();
}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, 
							class AController * EventInstigator, AActor * DamageCauser)  
{
	const float HealthLeft = CurrentHealth - DamageAmount; 
	if (HealthLeft <= 0.f)
	{
		CurrentHealth = 0.f;	

		Die();
	}
	else
	{
		CurrentHealth = HealthLeft;	
	}

	return DamageAmount;
}

void AShooterCharacter::Die()
{
	if (bDead) return;
	bDead = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();		
	if (!AnimInstance) return;

	if (!DeathMontage)
	{
		LOG_FSTRING_ON_SCREEN("DeathMontage is not set on player.")
		return;		
	}

	AnimInstance->Montage_Play(DeathMontage);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (!PlayerController) return;
	PlayerController->SetIgnoreMoveInput(true);	
	PlayerController->SetIgnoreLookInput(true);

	AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(PlayerController);
	if (!ShooterPlayerController || !GameOverWidgetClass) return;

	ShooterPlayerController->HideHUDOverlay();
	ShooterPlayerController->SetInputMode(FInputModeUIOnly());
	ShooterPlayerController->SetShowMouseCursor(true);
	if (UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass))
	{
		Widget->AddToViewport();
	}
}

void AShooterCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;	
}

void AShooterCharacter::MoveCameraAfterDeath(float DeltaTime)
{
    if (CameraSpringArm->TargetArmLength < TargetCameraSpringArmLengthAfterDeath)
    {
        float NewLength = FMath::FInterpTo(
        	CameraSpringArm->TargetArmLength,
        	TargetCameraSpringArmLengthAfterDeath, 
        	DeltaTime,
        	CameraZoomOutSpeedAfterDeath);

        CameraSpringArm->TargetArmLength = NewLength;
    }
}

float AShooterCharacter::GetCrosshairSpreadMultiplier(float DeltaTime)
{
	CalculateCrosshairVelocityFactor();
	CalculateCrosshairInAirFactor(DeltaTime);
	CalculateCrosshairAimFactor(DeltaTime);
	CalculateCrosshairFireFactor(DeltaTime);

	return CrosshairVelocityFactor + CrosshairInAirFactor + CrosshairAimFactor + CrosshairFireFactor+ 0.5f;
}

void AShooterCharacter::CalculateCrosshairVelocityFactor()
{
	const FVector2d WalkSpeedRange(0.f, GetCharacterMovement()->MaxWalkSpeed);
	const FVector2d VelocityMultiplierRange(0.f, 1.f);
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
}

void AShooterCharacter::CalculateCrosshairInAirFactor(float DeltaTime)
{
	if (GetMovementComponent()->IsFalling())
	{
		const float InAirTarget = 2.25f;
		const float InterpSpeed = 2.25f;
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, InAirTarget, DeltaTime, InterpSpeed);	
	}
	else
	{
		const float InterpSpeed = 10.f;
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, InterpSpeed);	
	}
}

void AShooterCharacter::CalculateCrosshairAimFactor(float DeltaTime)
{
	if (bAiming)
	{
		const float AimTarget = -0.4f;
		const float InterpSpeed = 2.25f;
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, AimTarget, DeltaTime, InterpSpeed);
	}
	else
	{
		const float InterpSpeed = 10.f;
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, InterpSpeed);
	}
}

void AShooterCharacter::CalculateCrosshairFireFactor(float DeltaTime)
{
	if (bCrosshairSpreadOnFire)
	{
		const float FireTarget = 0.3f;
		const float InterpSpeed = 15.f;
		CrosshairFireFactor = FMath::FInterpTo(CrosshairFireFactor, FireTarget, DeltaTime, InterpSpeed);
	}
	else
	{
		const float InterpSpeed = 15.f;
		CrosshairFireFactor = FMath::FInterpTo(CrosshairFireFactor, 0.f, DeltaTime, InterpSpeed);
	}
}

void AShooterCharacter::IncrementOverlappedItemCount(const int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void AShooterCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix{{0, Rotation.Yaw, 0}}.GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix{{0, Rotation.Yaw, 0}}.GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	const float TurnRate = bAiming ? AimingTurnRate : HipTurnRate;
	AddControllerYawInput(Rate * TurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	const float LookUpRate = bAiming ? AimingLookUpRate : HipLookUpRate;
	AddControllerPitchInput(Rate * LookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::TurnMouse(float MouseValue)
{
	// Mouse Turn Rates are stored in the game instance so that they can be accessed from the options menu
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance) return;

	const float TurnRate = bAiming ? GameInstance->MouseAimingTurnRate : GameInstance->MouseHipTurnRate;
	AddControllerYawInput(MouseValue * TurnRate);

}

void AShooterCharacter::LookUpMouse(float MouseValue)
{
	// Mouse Lookup Rates are stored in the game instance so that they can be accessed from the options menu
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance) return;

	const float LookUpRate = bAiming ? GameInstance->MouseAimingLookUpRate : GameInstance->MouseHipLookUpRate;
	AddControllerPitchInput(MouseValue * LookUpRate);

}

void AShooterCharacter::SetWalkingSpeedBasedOnWeaponType(AWeapon* Weapon)
{
	switch(Weapon->WeaponType)
	{
	case EWeaponType::EWT_Shotgun:
		GetCharacterMovement()->MaxWalkSpeed = ShotgunWalkSpeed;
		break;
	case EWeaponType::EWT_Rifle:
		GetCharacterMovement()->MaxWalkSpeed = RifleWalkSpeed;
		break;
	default:
		break;
	}
}

void AShooterCharacter::SetJumpVelocityBasedOnWeaponType(AWeapon* Weapon)
{
	switch(Weapon->WeaponType)
	{
	case EWeaponType::EWT_Shotgun:
		GetCharacterMovement()->JumpZVelocity = ShotgunJumpHeight; 
		break;
	case EWeaponType::EWT_Rifle:
		GetCharacterMovement()->JumpZVelocity = RifleJumpHeight;
		break;
	default:
		break;
	}
}

void AShooterCharacter::Reload()
{
	if (!EquippedWeapon || CombatState != ECombatState::ECS_Unoccupied || !IsCarryingRightAmmoType()) return;
	if (EquippedWeapon->AmmoCount >= EquippedWeapon->MagazineCapacity) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	switch(EquippedWeapon->WeaponType)
	{
	case EWeaponType::EWT_Shotgun:
		if (!ShotgunReloadMontage) return;
		AnimInstance->Montage_Play(ShotgunReloadMontage); //Will call FinishReloading()
		break;
	case EWeaponType::EWT_Rifle:
		if (!RifleReloadMontage) return;
		AnimInstance->Montage_Play(RifleReloadMontage); //Will call FinishReloading()
		break;
	default:
		break;
	}
	AnimInstance->Montage_JumpToSection(FName(TEXT("Reload")));

	CombatState = ECombatState::ECS_Reloading;
}

void AShooterCharacter::FinishReloading()
{
	CombatState = ECombatState::ECS_Unoccupied;
	
	
	const EAmmoType AmmoType = EquippedWeapon->AmmoType;
	if (!AmmoMap.Contains(AmmoType)) return;
	
	int32 CarriedAmmo = AmmoMap[AmmoType];
	const int32 MagEmptySpace = EquippedWeapon->MagazineCapacity - EquippedWeapon->AmmoCount;

	if (MagEmptySpace > CarriedAmmo)
	{
		// Reload with all the ammo we're carrying
		EquippedWeapon->ReloadAmmo(CarriedAmmo);
		CarriedAmmo = 0;
		AmmoMap.Add(AmmoType, CarriedAmmo);
	}
	else
	{
		// Fill the magazine
		EquippedWeapon->ReloadAmmo(MagEmptySpace);
		CarriedAmmo -= MagEmptySpace;
		AmmoMap.Add(AmmoType, CarriedAmmo);
	}

	if (bFireButtonPressed) Fire();
}

void AShooterCharacter::GrabClip()
{
	if (!EquippedWeapon) return;

	int32 ClipBoneIndex = EquippedWeapon->ItemMesh->GetBoneIndex(EquippedWeapon->MagazineBoneName);
	ClipTransform = EquippedWeapon->ItemMesh->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("hand_l")));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->bMagazineMoving = true;
}

void AShooterCharacter::ReleaseClip()
{
	EquippedWeapon->bMagazineMoving = false;
}

void AShooterCharacter::PlayMeleeImpactSound()
{
	if (MeleeImpactSound && !bMeleeImpactTimerInProgress)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, 
			MeleeImpactSound,
			GetActorLocation());

		GetWorldTimerManager().SetTimer(
			MeleeImpactSoundTimer, 
			this, 
			&AShooterCharacter::MeleeImpactSoundTimerReset, 
			MeleeImpactSoundWaitDuration);

		bMeleeImpactTimerInProgress = true;
	}
}

void AShooterCharacter::PlayBloodParticles(const FTransform Transform)
{
	if (BloodParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticles, Transform);
	}	
}

void AShooterCharacter::AimButtonPressed()
{
	bAiming = true;
}

void AShooterCharacter::AimButtonReleased()
{
	bAiming = false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	Fire();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::SelectButtonPressed()
{
	if (!TraceHitItem) return;
	
	if (AWeapon* Weapon = Cast<AWeapon>(TraceHitItem))
	{
		PickUpWeapon(Weapon);
	}
	else if (AAmmo* Ammo = Cast<AAmmo>(TraceHitItem))
	{
		PickupAmmo(Ammo);
	}
	else if (ALevelFinishItem* LevelFinishItem = Cast<ALevelFinishItem>(TraceHitItem)) 
	{
		LevelFinishItem->EndLevel();	
	}
	
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;	
}

void AShooterCharacter::ReloadButtonPressed()
{
	Reload();
}

void AShooterCharacter::OneKeyPressed()
{
	if (EquippedWeapon->InventorySlotIndex == 0) return;
	EquipWeaponFromInventory(0);
}

void AShooterCharacter::TwoKeyPressed()
{
	if (EquippedWeapon->InventorySlotIndex == 1) return;
	EquipWeaponFromInventory(1);
}

void AShooterCharacter::CheckInAirTime()
{
	bool bFallingDown = GetMovementComponent()->IsFalling() && GetMovementComponent()->Velocity.Z < 0;

	if (bFallingDown)
	{
		if (InAirTime == 0.f)
		{
			InAirTime = FPlatformTime::Seconds();
		}
	}
	else
	{
		if (InAirTime > 0.f)
		{
			float Elapsed = FPlatformTime::Seconds() - InAirTime;
			InAirTime = 0.f;
			if (Elapsed > HealthyFallTime)	
			{
				float Damage = Elapsed * 80.f;
				UGameplayStatics::ApplyDamage(
					this, 
					Damage, 
					this->GetController(), 
					this, 
					UDamageType::StaticClass());
			}
		}
	}
}

void AShooterCharacter::ThreeKeyPressed()
{
	if (EquippedWeapon->InventorySlotIndex == 2) return;
	EquipWeaponFromInventory(2);
}

void AShooterCharacter::Fire()
{
	if(CombatState != ECombatState::ECS_Unoccupied || !EquippedWeapon || bDead) return;

	bool bMiss = false;
	if (!EquippedWeapon->PullTrigger(bMiss)) return;

	PlayHipFireMontage();
	StartCrosshairFireSpreadTimer();
	StartFireTimer();
}

void AShooterCharacter::PlayHipFireMontage() const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(TEXT("Fire"));
	}
}

void AShooterCharacter::SetFireMontageBasedOnWeaponType(AWeapon* Weapon)
{
	switch (Weapon->WeaponType)
	{
	case EWeaponType::EWT_Shotgun:
		HipFireMontage = ShotgunHipFireMontage;
		break;
	case EWeaponType::EWT_Rifle:
		HipFireMontage = RifleHipFireMontage;
		break;
	default:
		break;
	}
}

void AShooterCharacter::StartFireTimer()
{
	if (!EquippedWeapon) return;

	CombatState = ECombatState::ECS_FireTimerInProgress;
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, EquippedWeapon->AutoFireWaitDuration);
}

void AShooterCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;
	
	if (EquippedWeapon->AmmoCount <= 0)
	{
		Reload();	
		return;
	}
	
	if (bFireButtonPressed)
	{
		Fire();	
	}
}
void AShooterCharacter::SetCameraFOVBasedOnAiming(float DeltaTime)
{
	if (bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
		FollowCamera->SetFieldOfView(CameraCurrentFOV);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
		FollowCamera->SetFieldOfView(CameraCurrentFOV);
	}
}

void AShooterCharacter::StartCrosshairFireSpreadTimer()
{
	bCrosshairSpreadOnFire = true;
	GetWorldTimerManager().SetTimer(
		CrosshairFireSpreadTimer,
		this,
		&AShooterCharacter::FinishCrosshairFireSpreadTimer,
		CrosshairSpreadDurationOnFire);
}

void AShooterCharacter::FinishCrosshairFireSpreadTimer()
{
	bCrosshairSpreadOnFire = false;
}

FVector2d AShooterCharacter::GetCrosshairLocation() const
{
	FVector2d ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	
	FVector2d CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	CrosshairLocation.Y -= CrosshairUpwardOffset;
	
	return CrosshairLocation;
}

void AShooterCharacter::TraceForItems()
{
	if (!bShouldTraceForItems)
	{
		if (TraceHitItemLastFrame) TraceHitItemLastFrame->SetPickupWidgetVisibility(false);
		return;
	}
	
	FHitResult ItemTraceResult;
	FVector2d CrosshairLocation = GetCrosshairLocation();

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
	FVector End = Start + CrosshairWorldDirection * ItemTraceDistance;

	if (bDeprojectSuccess)
	{
		GetWorld()->LineTraceSingleByChannel(ItemTraceResult, Start, End, ECC_Visibility);
	}

	if(!ItemTraceResult.bBlockingHit) return;
	
	TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
	if (TraceHitItem)
	{
		TraceHitItem->SetPickupWidgetVisibility(true);
		TraceHitItem->bInventoryIsFull = Inventory.Num() >= INVENTORY_SIZE ? true : false;
	}

	if (TraceHitItemLastFrame && TraceHitItemLastFrame != TraceHitItem)
	{
		TraceHitItemLastFrame->SetPickupWidgetVisibility(false);
	}
	
	TraceHitItemLastFrame = TraceHitItem;
}

void AShooterCharacter::PickUpWeapon(AWeapon* Weapon)
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	
	if (Inventory.Num() < INVENTORY_SIZE && !Inventory.Contains(Weapon))
	{
		Inventory.Add(Weapon);
		Weapon->SetOwner(this);
		Weapon->InventorySlotIndex = Inventory.Num() - 1;
		Weapon->UpdateItemStateAndProperties(EItemState::EIS_PickedUp);
	}
	else // Swap with equipped weapon
	{
		Inventory[EquippedWeapon->InventorySlotIndex] = Weapon;
		Weapon->InventorySlotIndex = EquippedWeapon->InventorySlotIndex;
		DropWeapon();
		EquipWeapon(Weapon);
	}

	ExtractExtraAmmoIntoAmmoMap(Weapon);
	if (Weapon->PickupSound) UGameplayStatics::PlaySound2D(GetWorld(), Weapon->PickupSound);
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!WeaponToEquip && CombatState != ECombatState::ECS_Unoccupied) return;

	CombatState = ECombatState::ECS_Equipping;
	WeaponToEquip->UpdateItemStateAndProperties(EItemState::EIS_Equipped);
	
	FName GunSocketName = WeaponToEquip->GunSocketName;
	if(const USkeletalMeshSocket* GunSocket = GetMesh()->GetSocketByName(GunSocketName))
	{
		GunSocket->AttachActor(WeaponToEquip, GetMesh());
	}
	
	if (WeaponToEquip->EquipSound) UGameplayStatics::PlaySound2D(GetWorld(), WeaponToEquip->EquipSound);

	const int32 CurrentSlotIndex = EquippedWeapon ? EquippedWeapon->InventorySlotIndex : 0;
	const int32 NewSlotIndex = WeaponToEquip->InventorySlotIndex; 
	EquipItemDelegate.Broadcast(CurrentSlotIndex, NewSlotIndex);

	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(EquipMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("Equip"));
	}

	SetFireMontageBasedOnWeaponType(WeaponToEquip);
	SetWalkingSpeedBasedOnWeaponType(WeaponToEquip);
	SetJumpVelocityBasedOnWeaponType(WeaponToEquip);

	EquippedWeapon = WeaponToEquip;
}

void AShooterCharacter::FinishEquipping()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void AShooterCharacter::DropWeapon()
{
	if (!EquippedWeapon && CombatState != ECombatState::ECS_Unoccupied) return;
	
	FVector ImpulseDirection = GetMesh()->GetRightVector().RotateAngleAxis(-20.f, GetMesh()->GetForwardVector());
	float ImpulsePower = 15'000.f;
	EquippedWeapon->ThrowWeapon(ImpulseDirection, ImpulsePower);
	EquippedWeapon = nullptr;
}

void AShooterCharacter::EquipWeaponFromInventory(int8 SlotIndex)
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	
	if (Inventory.Num() <= SlotIndex || Inventory[SlotIndex] == nullptr) return;

	EquippedWeapon->UpdateItemStateAndProperties(EItemState::EIS_PickedUp);
	if (AWeapon* Weapon = Cast<AWeapon>(Inventory[SlotIndex]))
	{
		EquipWeapon(Weapon);
	}
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_Shotgun, StartingShotgunAmmo);
	AmmoMap.Add(EAmmoType::EAT_Rifle, StartingRifleAmmo);
}

bool AShooterCharacter::IsCarryingRightAmmoType()
{
	if (!EquippedWeapon) return false;

	const EAmmoType AmmoType = EquippedWeapon->AmmoType;

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	
	return false;
}

void AShooterCharacter::ExtractExtraAmmoIntoAmmoMap(AWeapon* Weapon)
{
	if (Weapon->AmmoCount > Weapon->MagazineCapacity)
	{
		const EAmmoType AmmoType = Weapon->AmmoType;
		const int ExtraAmmo = Weapon->AmmoCount - Weapon->MagazineCapacity;
		if (AmmoMap.Find(AmmoType))
		{
			AmmoMap[AmmoType] += ExtraAmmo;
		}
		else
		{
			AmmoMap.Add(AmmoType, ExtraAmmo); 
		}
		Weapon->AmmoCount = Weapon->MagazineCapacity;	
	}
}

void AShooterCharacter::PickupAmmo(AAmmo* Ammo)
{
	if (Ammo->PickupSound) UGameplayStatics::PlaySound2D(GetWorld(), Ammo->PickupSound);
	
	// Update AmmoMap
	if (AmmoMap.Find(Ammo->AmmoType))
	{
		int AmmoCount = AmmoMap[Ammo->AmmoType];
		AmmoCount += Ammo->ItemCount;
		AmmoMap[Ammo->AmmoType] = AmmoCount;
	}
	else
	{
		AmmoMap.Add(Ammo->AmmoType, Ammo->ItemCount);
	}

	if (EquippedWeapon && EquippedWeapon->AmmoType == Ammo->AmmoType && EquippedWeapon->AmmoCount == 0)
	{
		Reload();	
	}

	Ammo->Destroy();
}

