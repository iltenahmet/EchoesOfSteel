// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "ShooterCharacter.h"

AAmmo::AAmmo()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);

	CollisionCapsule->SetupAttachment(GetRootComponent());
	PickupWidget->SetupAttachment(GetRootComponent());
	AreaSphere->SetupAttachment(GetRootComponent());

	AmmoCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AmmoCollisionSphere"));
	AmmoCollisionSphere->SetupAttachment(GetRootComponent());
}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	AmmoCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnAmmoSphereOverlap);
}

void AAmmo::UpdateItemProperties()
{
	Super::UpdateItemProperties();

	switch (ItemState)
	{
	case EItemState::EIS_Pickup:
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
		
	case EItemState::EIS_Equipped:
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
		
	case EItemState::EIS_Falling:
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoMesh->SetSimulatePhysics(true);
		AmmoMesh->SetEnableGravity(true);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		break;
	}
	
}

void AAmmo::OnAmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (!ShooterCharacter) return;

	ShooterCharacter->PickupAmmo(this);
	AmmoCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
