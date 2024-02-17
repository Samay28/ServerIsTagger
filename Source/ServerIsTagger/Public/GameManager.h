// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

UCLASS()
class SERVERISTAGGER_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FVector DestinationLocation;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool GameStarted;


	UFUNCTION(BlueprintCallable)
	void StartGame();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	class APlayerCharacter *Player;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FVector ServerLocation;

	UFUNCTION()
	FVector InititateTeleporting();

	UPROPERTY(EditAnywhere)
	TArray<FVector> TeleportLocations;

	UPROPERTY()
	TArray<float> PlayerRemDistances;

	UFUNCTION(BlueprintCallable)
	void CalculateResults();
};
