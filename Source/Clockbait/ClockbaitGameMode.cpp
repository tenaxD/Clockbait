// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClockbaitGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "ClockbaitPlayerController.h"
#include "ClockbaitGameState.h"
#include "Clockbait.h"
#include "Net/UnrealNetwork.h"
#include "ClockbaitGameState.h"

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

void AClockbaitGameMode::ShowGameOverToAll()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AClockbaitPlayerController* PlayerController = Cast<AClockbaitPlayerController>(It->Get()))
		{
			PlayerController->ClientShowGameOver();
		}
	}
}

void AClockbaitGameMode::NotifyPlayerCaught(APawn* CaughtPawn)
{
	if (bGameOverTriggered)
	{
		return;
	}
	bGameOverTriggered = true;

	UE_LOG(LogClockbait, Log, TEXT("NotifyPlayerCaught: '%s' was caught. Triggering game over for all players in %f seconds."),
		*GetNameSafe(CaughtPawn), KillCamDuration);

	GetWorldTimerManager().SetTimer(GameOverShowTimerHandle, this,
		&AClockbaitGameMode::ShowGameOverToAll, KillCamDuration, false);

}

void AClockbaitGameMode::TravelToCurrentLevel()
{
	GetWorld()->ServerTravel(TEXT("/Game/MyStuff/Maps/L_Main"));
}

void AClockbaitGameMode::NotifyPlayerReady(APlayerController* PC)
{
	if (AClockbaitGameState* GS = GetGameState<AClockbaitGameState>()) {
		if (PlayersReady.Contains(PC)) return;
		PlayersReady.Add(PC);
		GS->ReadyCount++;
		const int32 NumPlayers = GetNumPlayers();
		if (NumPlayers >= 2 && GS->ReadyCount >= NumPlayers) {
			GetWorldTimerManager().SetTimer
			(
				GameOverResetTimerHandle,
				this,
				&AClockbaitGameMode::TravelToCurrentLevel,
				GameOverResetDelay,
				false
			);
		}
	}
}
