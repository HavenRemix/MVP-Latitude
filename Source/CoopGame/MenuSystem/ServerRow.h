// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/TextBlock.h"
#include "Components/Button.h"


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API UServerRow : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerName;
	
	UFUNCTION()
	void Setup(class UMainMenu* InParent, uint32 InIndex);

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* RowButton;

	uint32 Index;

	UPROPERTY()
	class UMainMenu* Parent;

	UFUNCTION()
	void OnClicked();
};
