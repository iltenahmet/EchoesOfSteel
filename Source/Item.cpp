// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "ShooterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	CollisionCapsule->SetupAttachment(GetRootComponent());
	CollisionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionCapsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	UpdateItemStateAndProperties(EItemState::EIS_Pickup);
	SetPickupWidgetVisibility(false);	
	SetActiveStarsBasedOnPower();

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::SetPickupWidgetVisibility(bool Visibility)
{
	if (!PickupWidget) return;
	PickupWidget->SetVisibility(Visibility);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor))
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
    {
    	if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor))
    	{
    		ShooterCharacter->IncrementOverlappedItemCount(-1);
    	}
    }
}

void AItem::SetActiveStarsBasedOnPower()
{
	int8 StarsToAdd = ItemPower/20 + 1; // 0-19 = 1 star, 20-39 = 2 stars, and so on	
	for (int8 i = 0; i < 5; i++)
	{
		bool StarValue = i <= StarsToAdd-1 ? true : false;
		ActiveStars.Add(StarValue);
	}
}

void AItem::UpdateItemStateAndProperties(EItemState State)
{
	ItemState = State;
	UpdateItemProperties();
}

void AItem::UpdateItemProperties()
{
	switch (ItemState)
	{
	case EItemState::EIS_Pickup:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionCapsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
		
	case EItemState::EIS_Equipped:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetPickupWidgetVisibility(false);
		break;
		
	case EItemState::EIS_Falling:
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		
		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionCapsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_PickedUp:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetPickupWidgetVisibility(false);
		break;
	}
}
