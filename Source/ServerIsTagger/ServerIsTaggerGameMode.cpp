// Copyright Epic Games, Inc. All Rights Reserved.

#include "ServerIsTaggerGameMode.h"
#include "ServerIsTaggerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AServerIsTaggerGameMode::AServerIsTaggerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_MyPlayer"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
}

void AServerIsTaggerGameMode::HostLanGame()
{
	TArray<FString> MapPaths = {
		"/Game/Maps/GameMap?listen",
		"/Game/Maps/GameMap1?listen",
		"/Game/Maps/GameMap2?listen",
		"/Game/Maps/GameMap3?listen",
		"/Game/Maps/GameMap4?listen"};
	FString SelectedMap = MapPaths[FMath::RandRange(0, MapPaths.Num() - 1)];
	GetWorld()->ServerTravel("/Game/Maps/GameMap?listen");
	UE_LOG(LogTemp, Warning, TEXT("%s"), *SelectedMap);
}
void AServerIsTaggerGameMode::JoinLanGame(FString IP)
{
	APlayerController *Pc = GetGameInstance()->GetFirstLocalPlayerController();
	if (Pc)
	{
		Pc->ClientTravel(IP, TRAVEL_Absolute);
	}
}

