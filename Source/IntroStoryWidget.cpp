// Fill out your copyright notice in the Description page of Project Settings.


#include "IntroStoryWidget.h"
#include "Components/TextBlock.h" 
#include "Animation/WidgetAnimation.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h" 
#include "TimerManager.h"
#include "Macros.h"

void UIntroStoryWidget::PlayAnimations(int32 AnimIndex)
{
	UWidgetAnimation* Anim = Animations[AnimIndex];
	UTextBlock* Text = Texts[AnimIndex];

	Text->SetVisibility(ESlateVisibility::Visible);
	this->PlayAnimation(Anim);
	float EndTime = Anim->GetEndTime();

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("OnAnimationEnd"), Text, AnimIndex+1);
	bool bInLoop = false;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, EndTime, bInLoop);
}

void UIntroStoryWidget::OnAnimationEnd(UTextBlock* Text, int32 NextAnimIndex)
{
	Text->SetVisibility(ESlateVisibility::Hidden);

	if (NextAnimIndex < Animations.Num())
	{
		PlayAnimations(NextAnimIndex);
	} 
	else
	{
		EndAnimDelegate.Broadcast();
	}
}
