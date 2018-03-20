// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MenuSystem/MenuInterface.h"


#include "CoreMinimal.h"
#include "OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "SGameInstance.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class COOPGAME_API USGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

private:

	TSubclassOf<class UUserWidget> MenuClass;

	class UMainMenu* Menu;

	FString DesiredServerName;

//------- MULTIPLAYER ------- \\


	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool Success);

	UFUNCTION()
	void OnDestroySessionComplete(FName SessionName, bool Success);

	UFUNCTION()
	void OnFindSessionComplete(bool Success);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void CreateSession();

public:

	USGameInstance(const FObjectInitializer & ObjectInitializer);
	virtual void Init();

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();


//------- MULTIPLAYER ------- \\


	UFUNCTION(Exec)
	void Host(FString ServerName) override;

	UFUNCTION(Exec)
	void Join(uint32 Index) override;

	UFUNCTION()
	void RefreshServerList() override;

};
