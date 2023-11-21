// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNTITLEDTPS_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void ShowHUDOverlay();

	UFUNCTION(BlueprintCallable)
	void HideHUDOverlay();

	/** Reference to the Overall HUD Overlay Blueprint Class*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<class UUserWidget> HUDOverlayClass;

protected:
	/** Variable to hold the HUD Overlay Widget after creating it */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widgets)
	UUserWidget* HUDOverlay;
};
