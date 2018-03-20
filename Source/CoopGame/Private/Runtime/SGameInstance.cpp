// Fill out your copyright notice in the Description page of Project Settings.

#include "Runtime/SGameInstance.h"
#include "MenuSystem/MenuWidget.h"
#include "MenuSystem/MainMenu.h"

#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"


const static FName SESSION_NAME = TEXT("My Session Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");


USGameInstance::USGameInstance(const FObjectInitializer & ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/UI/Menus/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;
	MenuClass = MenuBPClass.Class;
}


void USGameInstance::Init()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found: %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &USGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USGameInstance::OnJoinSessionComplete);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Found: None"));
	}
}


//------- MULTIPLAYER FUNCTIONS ------- \\


//Move The Player To The Map
void USGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could Not Make Session!"));
		return;
	}
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->ServerTravel("/Game/Maps/Showdown/Blockout_P?listen");
}


//Destroy A Session
void USGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();
	}
}


//Find Session Complete Callback
void USGameInstance::OnFindSessionComplete(bool Success)
{
	if (Success && SessionSearch.IsValid() && Menu != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Session Search!"));

		TArray<FServerData> ServerNames;
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			FServerData Data;
			Data.CurrentPlayers = SearchResult.Session.NumOpenPublicConnections;
			ServerNames.Add(Data);

			FString ServerName;

			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				Data.Name = ServerName; 
			}
			else {
				Data.Name = "Could Not Access The Server Name. Error 10122.";
			}

			Menu->SetServerList(ServerNames);
		}
	}
}


//Get Host Of An Address And The Join The Session
void USGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could Not Recieve Connect String"));
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}


//Create The Session
void USGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
			if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
			{
				SessionSettings.bIsLANMatch = true;
			}
			else {
				SessionSettings.bIsLANMatch = false;
			}
			SessionSettings.NumPublicConnections = 4;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bUsesPresence = true;
			SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}


//Host a game as the server
void USGameInstance::Host(FString ServerName)
{
	DesiredServerName = ServerName;
	if (SessionInterface.IsValid())
	{
		if (Menu != nullptr)
		{
			Menu->Teardown();
		}

		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else {
			CreateSession();
		}
	}
}


//Join a game as the client
void USGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	if (Menu != nullptr)
	{
		//Menu->SetServerList({"Test 1", "Test 2"});
		Menu->Teardown();
	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}


//Refresh The Server List
void USGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionSearch->MaxSearchResults = 1000;

		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}


// ------- UMG ------- \\


void USGameInstance::LoadMainMenu()
{
	//Make MainMenu Widget
	if (!ensure(MenuClass != nullptr)) return;
	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;
	
	//Rest Of Setup Is In Menu Class
	Menu->Setup();

	//Sets Game Instance As Menu Interface
	Menu->SetMenuInterface(this);
}
