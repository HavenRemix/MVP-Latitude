// Fill out your copyright notice in the Description page of Project Settings.


#include "SHUD.h"
#include "..\..\Public\HUD\SHUD.h"
#include "Blueprint/UserWidget.h"

void ASHUD::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (MainHUDWidgetClass)
	{
		MainHUDWidget = CreateWidget<UMainHUDWidget>(GetWorld(), MainHUDWidgetClass);
		if (MainHUDWidget)
		{
			MainHUDWidget->AddToViewport();
		}
	}
}


void ASHUD::DrawHUD()
{
}


void ASHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void ASHUD::UpdateCurrentAmmo(int32 CurrentAmmo)
{
}


void ASHUD::ResetCurrentAmmo()
{
}
