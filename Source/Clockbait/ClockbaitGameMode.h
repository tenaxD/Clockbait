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

protected:

	/** Distributes joining players across available PlayerStart actors instead of stacking them on one */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** Logs player connections so possession can be verified in the Output Log during PIE testing */
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:

	int32 NextPlayerStartIndex = 0;
};



