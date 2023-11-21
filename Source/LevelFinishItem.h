// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "LevelFinishItem.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API ALevelFinishItem : public AItem
{
	GENERATED_BODY()
	
public:
	ALevelFinishItem();

	UFUNCTION(BlueprintNativeEvent)
	void EndLevel();

	void EndLevel_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EndLevelText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<class UWorld> NextLevel;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;
};
