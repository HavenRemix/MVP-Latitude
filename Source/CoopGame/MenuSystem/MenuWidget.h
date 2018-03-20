// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MenuSystem/MenuInterface.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetMenuInterface(IMenuInterface* MenuInterface);

	void Setup();
	void Teardown();
	
protected:

	IMenuInterface* MenuInterface;

	
};