// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/ActionPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"

void AActionPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* SubSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (SubSystem && DefaultMappingContext)
	{
		SubSystem->AddMappingContext(DefaultMappingContext, GameInputPriority);
	}
	
	PlayerCameraManager->ViewPitchMax = ViewPitchMax;
	PlayerCameraManager->ViewPitchMin = ViewPitchMin;
	
}

void AActionPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AActionPlayerController::OnLookInput);
	}
}

void AActionPlayerController::OnInventoryOpenClose(bool bInventoryOpen)
{
	// FInputModeGameOnly;	// 기본 상태
	// FInputModeUIOnly;	// 
	// FInputModeGameAndUI;

	if (bInventoryOpen)
	{
		FInputModeGameOnly InputModeGame;
		SetInputMode(InputModeGame);
	}
	else
	{
		FInputModeUIOnly InputModeUI;
		SetInputMode(InputModeUI);
	}
}

void AActionPlayerController::OnLookInput(const FInputActionValue& InValue)
{
	FVector2D LookAxis = InValue.Get<FVector2D>();
	AddYawInput(LookAxis.X);
	AddPitchInput(LookAxis.Y);

	//UE_LOG(LogTemp, Log, TEXT("(%.1f, %.1f)"), LookAxis.X, LookAxis.Y);
}
