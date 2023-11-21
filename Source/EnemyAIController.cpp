// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "EnemyMelee.h"
#include "EnemyShooter.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "BehaviorTree/BehaviorTreeComponent.h" 
#include "BehaviorTree/BehaviorTree.h"
#include "Macros.h"

AEnemyAIController::AEnemyAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemy* Enemy = Cast<AEnemyMelee>(InPawn);
	if (!Enemy)
	{
		Enemy = Cast<AEnemyShooter>(InPawn);
	}

	if (Enemy && Enemy->BehaviorTree)
	{
		BlackboardComponent->InitializeBlackboard(*(Enemy->BehaviorTree->BlackboardAsset));
	}
}
