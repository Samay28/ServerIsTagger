// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManager.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

// Sets default values
AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	QualifyingPlayers = 5;
	PlayersLeft = 10;
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Player->HasAuthority())
	{
		ServerLocation = Player->GetActorLocation();
	}
}
void AGameManager::StartGame()
{
	if (HasAuthority())
	{
		GameStarted = true;
		Player->CanStartOverlapping();
		UE_LOG(LogTemp, Warning, TEXT("Called"));
		GetWorldTimerManager().SetTimer(ResultsHandle, this, &AGameManager::CalculateResults, 600.0f, false);
	}
}

void AGameManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGameManager, GameStarted);
	DOREPLIFETIME(AGameManager, ServerLocation);
	DOREPLIFETIME(AGameManager, PlayersLeft);
}

FVector AGameManager::InititateTeleporting()
{
	int32 RandomIndex = FMath::RandRange(0, TeleportLocations.Num() - 1);
	return TeleportLocations[RandomIndex];
}

void AGameManager::CalculateResults()
{
	// Get all player controllers
	UWorld *World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<TPair<APlayerController *, float>> PlayerDistances;

	// Get the server player controller
	APlayerController *ServerPlayerController = World->GetFirstPlayerController();

	// Iterate over all player controllers
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController *PlayerController = It->Get();
		if (PlayerController && PlayerController->GetPawn() && PlayerController != ServerPlayerController)
		{
			float Distance = FVector::Distance(PlayerController->GetPawn()->GetActorLocation(), DestinationLocation);
			PlayerDistances.Add(TPair<APlayerController *, float>(PlayerController, Distance));
		}
	}

	if (PlayerDistances.Num() < 2)
	{
		return;
	}

	// quick Sort
	PlayerDistances.Sort([](const TPair<APlayerController *, float> &A, const TPair<APlayerController *, float> &B)
						 { return A.Value < B.Value; });

	// Get the top 2 nearest players
	TArray<APlayerController *> NearestPlayers;
	for(int i=0; i<QualifyingPlayers; i++)
	{
		NearestPlayers.Add(PlayerDistances[i].Key);
	}

	for (TPair<APlayerController *, float> &PlayerDistance : PlayerDistances)
	{
		APlayerCharacter *PlayerCharacter = Cast<APlayerCharacter>(PlayerDistance.Key->GetPawn());
		if (PlayerCharacter)
		{
			bool bIsQualified = NearestPlayers.Contains(PlayerDistance.Key);
			if (!bIsQualified)
			{
				PlayerDistance.Key->ClientReturnToMainMenuWithTextReason(FText::FromString("You are eliminated from the game"));
			}
		}
	}
}
