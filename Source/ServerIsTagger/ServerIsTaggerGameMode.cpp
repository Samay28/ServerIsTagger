// Copyright Epic Games, Inc. All Rights Reserved.

#include "ServerIsTaggerGameMode.h"
#include "ServerIsTaggerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AServerIsTaggerGameMode::AServerIsTaggerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AServerIsTaggerGameMode::HostLanGame()
{
	GetWorld()->ServerTravel("/Game/Maps/GameMap?listen");
}
void AServerIsTaggerGameMode::JoinLanGame()
{
	APlayerController* Pc = GetGameInstance()->GetFirstLocalPlayerController();
	if(Pc)
	{
		Pc->ClientTravel("192.168.1.7", TRAVEL_Absolute);
	}
}

