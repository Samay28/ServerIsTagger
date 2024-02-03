// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "AudioCaptureComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the SpringArm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh(), FName("head"));
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	Voice = CreateDefaultSubobject<UAudioCaptureComponent>(TEXT("Voice Capture"));
	Voice->SetupAttachment(Camera);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bReplicates = true;
	NetUpdateFrequency = 30.0f;
	GameStarted = false;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, bCanOverlap);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}

	bCanOverlap = false;
	UCapsuleComponent *CC = FindComponentByClass<UCapsuleComponent>();
	if (CC)
	{
		CC->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBegin);
	}
}

void APlayerCharacter::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (HasAuthority() && bCanOverlap)
	{
		APlayerCharacter *OtherPlayer = Cast<APlayerCharacter>(OtherActor);
		if (OtherPlayer && OtherPlayer != this)
		{
			// Only kick the other player if it's a valid player and not the server player itself
			if (!OtherPlayer->IsLocallyControlled())
			{
				APlayerController *OtherPlayerController = Cast<APlayerController>(OtherPlayer->GetController());
				if (OtherPlayerController)
				{
					FString KickReasonString = TEXT("You were kicked from the server.");
					FText KickReason = FText::FromString(KickReasonString);
					OtherPlayerController->ClientReturnToMainMenuWithTextReason(KickReason);
				}
			}
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);
		EnhancedInputComponent->BindAction(StartGameAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StartGame);
	}
}

void APlayerCharacter::Move(const FInputActionValue &Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue &Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::ServerSprint_Implementation()
{

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	OnRep_IsSprinting();

}

bool APlayerCharacter::ServerSprint_Validate()
{
	return true;
}

void APlayerCharacter::ServerStopSprint_Implementation()
{

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	OnRep_IsSprinting();
	
}

bool APlayerCharacter::ServerStopSprint_Validate()
{
	return true;
}
void APlayerCharacter::OnRep_IsSprinting()
{

}

void APlayerCharacter::Sprint()
{

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	ServerSprint();

}

void APlayerCharacter::StopSprint()
{

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	ServerStopSprint();

}

void APlayerCharacter::StartGame()
{
	if(HasAuthority())
	{
		GameStarted = true;
		UE_LOG(LogTemp,Warning,TEXT("Game started?"));
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)

{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
