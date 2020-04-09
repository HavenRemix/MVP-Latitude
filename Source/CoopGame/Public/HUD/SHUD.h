// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/Widget.h"
#include "HUD/SMainHUDWidget.h"
#include "SHUD.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASHUD : public AHUD
{
	GENERATED_BODY()

public:

	ASHUD();

	virtual void BeginPlay() override;

	virtual void DrawHUD() override;
	
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void UpdateCurrentAmmo(int32 CurrentAmmo);

	UFUNCTION()
	void ResetCurrentAmmo();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> MainHUDWidgetClass;

private:

	class UUserWidget* MainHUDWidget;
	
};
