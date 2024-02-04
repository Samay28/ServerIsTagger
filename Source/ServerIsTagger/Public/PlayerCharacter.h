// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class SERVERISTAGGER_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = Inputs)
	class UInputAction *LookAction;

	UPROPERTY(EditAnywhere, Category = Inputs)
	class UInputAction *MoveAction;

	UPROPERTY(EditAnywhere, Category = Inputs)
	class UInputAction *SprintAction;

	UPROPERTY(EditAnywhere, Category = Inputs)
	class UInputAction *StartGameAction;

	UPROPERTY(EditAnywhere, Category = Inputs)
	class UInputMappingContext *PlayerMappingContext;

	UPROPERTY(EditAnywhere,ReplicatedUsing = OnRep_GameStarted,BlueprintReadOnly)
	bool GameStarted;

	UFUNCTION()
	void OnRep_GameStarted();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent *SpringArm;

	UPROPERTY(EditAnywhere)
	class UCameraComponent *Camera;

	UPROPERTY(EditAnywhere)
	class UAudioCaptureComponent *Voice;

	void Move(const FInputActionValue &Value);
	void Look(const FInputActionValue &Value);
	void Sprint();
	void StopSprint();
	void StartGame();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void ServerStartGame();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopSprint();

	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting)
	bool bIsSprinting;

	UFUNCTION()
	void OnRep_IsSprinting();

	FVector StartLocation;

private:
	UPROPERTY(Replicated)
	bool bCanOverlap;
};
