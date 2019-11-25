// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	float Health;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FString PlayerName;

	UFUNCTION()
	FString GetPlayerName_SaveGame();

	UFUNCTION()
	void SetPlayerName_SaveGame(FString Name);
	
};
