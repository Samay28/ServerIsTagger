// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ServerIsTaggerGameMode.generated.h"

UCLASS(minimalapi)
class AServerIsTaggerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AServerIsTaggerGameMode();

	UFUNCTION(BlueprintCallable)
	void HostLanGame();


	UFUNCTION(BlueprintCallable)
	void JoinLanGame(FString IP);
};



