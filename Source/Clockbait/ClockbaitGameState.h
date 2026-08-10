// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ClockbaitGameState.generated.h"

/** Server-authoritative snapshot of one room's AI: where it is, and whether it's currently perceiving a player. */
USTRUCT(BlueprintType)
struct FClockbaitRoomAIState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName RoomId;

	// Net-quantized (10cm precision) since this replicates every tick the AI moves - full FVector
	// precision isn't needed for a minimap dot.
	UPROPERTY(BlueprintReadOnly)
	FVector_NetQuantize10 AILocation;

	UPROPERTY(BlueprintReadOnly)
	bool bIsPerceivingPlayer = false;
};

/**
 *  Replicates each room's AI position/state to every client, so a player in one room can see
 *  (via a minimap widget) where the entity is in their teammate's room.
 */
UCLASS()
class CLOCKBAIT_API AClockbaitGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Server-only: called every tick by BTService_UpdatePlayerLocation to publish where a room's AI currently is. */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdateRoomAIState(FName RoomId, FVector AILocation, bool bIsPerceivingPlayer);

	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<FClockbaitRoomAIState> RoomAIStates;
};
