// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelFinishItem.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

ALevelFinishItem::ALevelFinishItem()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	CollisionCapsule->SetupAttachment(GetRootComponent());
	PickupWidget->SetupAttachment(GetRootComponent());
	AreaSphere->SetupAttachment(GetRootComponent());
}

void ALevelFinishItem::EndLevel_Implementation()
{
}

