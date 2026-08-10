// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClockbaitGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "ClockbaitPlayerController.h"
#include "ClockbaitGameState.h"
#include "Clockbait.h"

AClockbaitGameMode::AClockbaitGameMode()
{
	GameStateClass = AClockbaitGameState::StaticClass();
}

AActor* AClockbaitGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() == 0)
	{
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	AActor* ChosenStart = PlayerStarts[NextPlayerStartIndex % PlayerStarts.Num()];
	NextPlayerStartIndex++;

	return ChosenStart;
}

void AClockbaitGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogClockbait, Log, TEXT("PostLogin: '%s' connected. Total connected players: %d"),
		*GetNameSafe(NewPlayer), GetNumPlayers());
}

void AClockbaitGameMode::NotifyPlayerCaught(APawn* CaughtPawn)
{
	if (bGameOverTriggered)
	{
		return;
	}
	bGameOverTriggered = true;

	UE_LOG(LogClockbait, Log, TEXT("NotifyPlayerCaught: '%s' was caught. Triggering game over for all players."),
		*GetNameSafe(CaughtPawn));

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AClockbaitPlayerController* PlayerController = Cast<AClockbaitPlayerController>(It->Get()))
		{
			PlayerController->ClientShowGameOver();
		}
	}

	GetWorldTimerManager().SetTimer(GameOverResetTimerHandle, this, &AClockbaitGameMode::ResetLevel, GameOverResetDelay, false);
}

void AClockbaitGameMode::ResetLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this, true)));
}
