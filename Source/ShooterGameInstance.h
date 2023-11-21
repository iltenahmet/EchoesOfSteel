// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Input)
	float MouseHipTurnRate = 1.2f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Input)
	float MouseHipLookUpRate = 1.2f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Input)
	float MouseAimingTurnRate = 0.7f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Input)
	float MouseAimingLookUpRate = 0.7f;
	
};
