// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AGameManager::StartGame()
{
	if (HasAuthority())
	{
		GameStarted = true;
		UE_LOG(LogTemp,Warning,TEXT("Called"));
	}
}

void AGameManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGameManager, GameStarted);
}

