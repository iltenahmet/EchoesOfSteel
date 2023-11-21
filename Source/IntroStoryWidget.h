// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IntroStoryWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndAnimDelegate);

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API UIntroStoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** PLays animations from the Animations array one by one, starting from the AnimIndex. First animation is index 0.*/	
	UFUNCTION(BlueprintCallable)
	void PlayAnimations(int32 AnimIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UWidgetAnimation*> Animations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UTextBlock*> Texts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)	
	bool bAnimationsDone = false;

	/** This delegate will broadcast once all animations have played */
	UPROPERTY(BlueprintAssignable)
	FEndAnimDelegate EndAnimDelegate;

protected:
	UFUNCTION()
	void OnAnimationEnd(UTextBlock* Text, int32 NextAnimIndex);
};
