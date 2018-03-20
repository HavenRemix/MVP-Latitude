// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MenuSystem/MenuInterface.h"

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "MainMenu.generated.h"


USTRUCT()
struct FServerData 
{
	GENERATED_BODY()

	FString Name;

	uint16 CurrentPlayers;
};


class UUserWidget;

/**
 * 
 */
UCLASS()
class COOPGAME_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

public:

	UMainMenu(const FObjectInitializer & ObjectInitializer);

	class UServerRow* Row;

	UFUNCTION()
	void SetServerList(TArray<FServerData> ServerNames);

	void SelectIndex(uint32 Index);

protected:

	virtual bool Initialize();

private:

	TSubclassOf<class UUserWidget> ServerRowClass;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;


// ------- MAIN MENU ------- \\


	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Host;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Join;

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void OpenJoinMenu();

// ------- JOIN MENU ------- \\

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_CancelJoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_JoinServer;

	UFUNCTION()
	void CloseJoinMenu();

	UFUNCTION()
	void JoinServer();

	TOptional<uint32> SelectedIndex;

// -------- HOST MENU ------- \\

	UFUNCTION()
	void OpenHostMenu();

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelHostServerButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostServerButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* GameNameField;

};
