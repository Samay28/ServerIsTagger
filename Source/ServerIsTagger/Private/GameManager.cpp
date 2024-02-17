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
	}
}

void AGameManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGameManager, GameStarted);
	DOREPLIFETIME(AGameManager, ServerLocation);
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
		// Handle the case where the world is not valid
		return;
	}

	// Create an array to store player distances
	TArray<TPair<APlayerController *, float>> PlayerDistances;

	// Iterate over all player controllers
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController *PlayerController = It->Get();
		if (PlayerController && PlayerController->GetPawn())
		{
			float Distance = FVector::Distance(PlayerController->GetPawn()->GetActorLocation(), DestinationLocation);
			PlayerDistances.Add(TPair<APlayerController *, float>(PlayerController, Distance));
		}
	}

	// Ensure there are at least 2 players
	if (PlayerDistances.Num() < 2)
	{
		// Handle the case where there are not enough players
		return;
	}

	// Sort the player distances array based on distances
	PlayerDistances.Sort([](const TPair<APlayerController *, float> &A, const TPair<APlayerController *, float> &B)
						 { return A.Value < B.Value; });

	// Get the top 2 nearest players
	TArray<APlayerController *> NearestPlayers;
	NearestPlayers.Add(PlayerDistances[0].Key);
	NearestPlayers.Add(PlayerDistances[1].Key);

	// Update UI text based on qualification status
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
