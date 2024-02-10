// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManager.h"
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
