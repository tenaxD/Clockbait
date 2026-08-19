// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ClockbaitGameMode.generated.h"

/**
 *  Simple GameMode for a first person game
 */
UCLASS(abstract)
class AClockbaitGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AClockbaitGameMode();

	/** Call (server-side) when the entity catches a player. Shows game over for every player, then resets the level for the whole session. */
	UFUNCTION(BlueprintCallable, Category = "GameOver")
	void NotifyPlayerCaught(APawn* CaughtPawn);

	void TravelToCurrentLevel();

protected:

	/** Distributes joining players across available PlayerStart actors instead of stacking them on one */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** Logs player connections so possession can be verified in the Output Log during PIE testing */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** How long to show the game over screen before the level resets for everyone */
	UPROPERTY(EditDefaultsOnly, Category = "GameOver", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float GameOverResetDelay = 4.0f;

private:

	int32 NextPlayerStartIndex = 0;

	bool bGameOverTriggered = false;

	FTimerHandle GameOverResetTimerHandle;
};



