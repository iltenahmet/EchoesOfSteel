// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HitNumberWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API UHitNumberWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Damage)
	float Damage = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Damage)
	int32 DamageTier = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UWidgetAnimation* SlideAndFadeAnimation;
};
