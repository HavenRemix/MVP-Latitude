// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGame.h"
#include "..\..\Public\Runtime\SSaveGame.h"


FString USSaveGame::GetPlayerName_SaveGame()
{
	return PlayerName;
}


void USSaveGame::SetPlayerName_SaveGame(FString Name)
{
	PlayerName = Name;
}
