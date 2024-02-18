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
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "AudioCaptureComponent.h"
#include "TimerManager.h"
#include "GameManager.h"

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

	GM = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
	if (!GM)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGameManager not found!"));
	}
	TeleportCounts = 2;
	TimesOverlapped = 0;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, bCanOverlap);
	DOREPLIFETIME(APlayerCharacter, TeleportCounts);
	DOREPLIFETIME(APlayerCharacter, TimesOverlapped);
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
	UCapsuleComponent *CC = FindComponentByClass<UCapsuleComponent>();
	if (CC)
	{
		CC->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBegin);
	}
	StartLocation = GetActorLocation();
	bCanOverlap = false;
	bReplicates = true;
}

void APlayerCharacter::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{

	if (HasAuthority() && bCanOverlap)
	{
		APlayerCharacter *OtherPlayer = Cast<APlayerCharacter>(OtherActor);
		if (OtherPlayer && OtherPlayer != this)
		{
			if (!OtherPlayer->IsLocallyControlled() && OtherPlayer->TimesOverlapped < 1)
			{
				FVector NewLocation = StartLocation;
				OtherPlayer->SetActorLocation(NewLocation);
				OtherPlayer->TimesOverlapped++;
			}
			else if (OtherPlayer->TimesOverlapped >= 1)
			{
				APlayerController *OtherPlayerController = OtherPlayer->GetController<APlayerController>();
				if (OtherPlayerController)
				{
					OtherPlayerController->ClientReturnToMainMenuWithTextReason(FText::FromString("You are kicked from the game"));
				}
			}
		}
	}
}

void APlayerCharacter::CanStartOverlapping()
{
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::StartOverlapping, 60.0f, false);
}

void APlayerCharacter::StartOverlapping()
{
	bCanOverlap = true;
	UE_LOG(LogTemp, Warning, TEXT("Done"));
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);
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
	if (HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = 700.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		ServerSprint();
	}
}

void APlayerCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	ServerStopSprint();
}

void APlayerCharacter::TeleportPlayer_Implementation()
{
	if (TeleportCounts > 0 && GM->GameStarted)
	{
		UE_LOG(LogTemp, Warning, TEXT("HAHA"));
		FVector RandomTeleportLocation = GM->InititateTeleporting();
		SetActorLocation(RandomTeleportLocation);
		TeleportCounts--;
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
