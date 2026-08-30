// Copyright Epic Games, Inc. All Rights Reserved.


#include "ClockbaitPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "ClockbaitCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "Clockbait.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "ClockbaitGameMode.h"

AClockbaitPlayerController::AClockbaitPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AClockbaitCameraManager::StaticClass();
}

void AClockbaitPlayerController::ExitPuzzleMode_Implementation(float InteractionBlockTime)
{
	SetViewTargetWithBlend(GetPawn(), InteractionBlockTime, EViewTargetBlendFunction::VTBlend_Cubic);
	FInputModeGameOnly PuzzleExitInputMode;
	SetInputMode(PuzzleExitInputMode);
	ResetIgnoreLookInput();
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	CurrentClickTraceChannel = ECC_Visibility;
}

void AClockbaitPlayerController::EnterPuzzleMode_Implementation(AActor* PuzzleStation, float InteractionBlockTime)
{
	SetViewTargetWithBlend(PuzzleStation, InteractionBlockTime, EViewTargetBlendFunction::VTBlend_Cubic);
	FInputModeGameAndUI PuzzleInputMode;
	SetInputMode(PuzzleInputMode);
	SetIgnoreLookInput(true);
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	CurrentClickTraceChannel = ClockbaitCollision::Interaction;
}

void AClockbaitPlayerController::ServerRequestRestart_Implementation()
{
	if (AClockbaitGameMode* GM = GetWorld()->GetAuthGameMode<AClockbaitGameMode>()) {
		GM->TravelToCurrentLevel();
	}
	
}

void AClockbaitPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogClockbait, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AClockbaitPlayerController::ClientShowGameOver_Implementation()
{
	OnGameOver();
}

void AClockbaitPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}
