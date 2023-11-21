// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController(); 

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(BlueprintReadWrite, Category = AI)
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	UPROPERTY(BlueprintReadWrite, Category = AI)
	class UBlackboardComponent* BlackboardComponent;

};
