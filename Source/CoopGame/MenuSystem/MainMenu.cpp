// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "MenuSystem/ServerRow.h"

#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"


UMainMenu::UMainMenu(const FObjectInitializer & ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/UI/RandomWidgets/RWBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	ServerRowClass = ServerRowBPClass.Class;
}


void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	ServerList->ClearChildren();

	uint32 i = 0;
	for (const FServerData& ServerName : ServerNames)
	{
		Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr)) return;

		Row->ServerName->SetText(FText::FromString(ServerName.Name));
		Row->Setup(this, i);
		i++;

		ServerList->AddChild(Row);
	}
}


void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
}


bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

//Main Menu

	if (!ensure(Button_Host != nullptr)) return false;
	Button_Host->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

	if (!ensure(Button_Join != nullptr)) return false;
	Button_Join->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

//Join Menu

	if (!ensure(Button_CancelJoinMenu != nullptr)) return false;
	Button_CancelJoinMenu->OnClicked.AddDynamic(this, &UMainMenu::CloseJoinMenu);

	if (!ensure(Button_JoinServer != nullptr)) return false;
	Button_JoinServer->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

//Host Menu

	if (!ensure(CancelHostServerButton != nullptr)) return false;
	CancelHostServerButton->OnClicked.AddDynamic(this, &UMainMenu::CloseJoinMenu);

	if (!ensure(HostServerButton != nullptr)) return false;
	HostServerButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	return true;
}


//Calls The Host Function On The Menu Interface
void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		FString ServerName = GameNameField->Text.ToString();
		MenuInterface->Host(ServerName);
	}
}


// ------- OPENING AND CLOSING MENUS ------- \\


void UMainMenu::OpenJoinMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);

	if (MenuInterface != nullptr)
	{
		MenuInterface->RefreshServerList();
	}
}


void UMainMenu::CloseJoinMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}


void UMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet() && MenuInterface != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index: %d"), SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index: Not Set"));
	}
}


void UMainMenu::OpenHostMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(HostMenu);
}
