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
	class UInputMappingContext *PlayerMappingContext;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent *SpringArm;

	UPROPERTY(EditAnywhere)
	class UCameraComponent *Camera;

	void Move(const FInputActionValue &Value);
	void Look(const FInputActionValue &Value);
	void Sprint();
	void StopSprint();

private:
};
