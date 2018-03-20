// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuWidget.h"


void UMenuWidget::SetMenuInterface(IMenuInterface * MenuInterface)
{
	this->MenuInterface = MenuInterface;
}


//Setup Menu
void UMenuWidget::Setup()
{
	//Add MainMenu To Viewport
	this->AddToViewport();

	//Get The World So We Can Get Player Controller
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	//Change Input Mode So We Can Use The MainManu
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);

	PlayerController->SetInputMode(InputModeData);

	//Set Show Mouse Cursor To True So We Can See And Control It
	PlayerController->bShowMouseCursor = true;
}


//Destroy Menu
void UMenuWidget::Teardown()
{
	//Remove MainMenu To Viewport
	this->RemoveFromViewport();

	//Get The World So We Can Get Player Controller
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	//Change Input Mode So We Can Use The MainManu
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);

	//Set Show Mouse Cursor To False So We Can Focus On The Game
	PlayerController->bShowMouseCursor = false;
}

