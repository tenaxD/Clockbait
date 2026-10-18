// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ClockbaitGameState.generated.h"

UENUM(BlueprintType)
enum class EEntityState : uint8 {
	Patrolling,
	Investigating,
	Chasing,
	Alarm,
	Frozen
};

/** Server-authoritative snapshot of one room's AI: where it is, and its state. */
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
	EEntityState EnemyState = EEntityState::Patrolling;
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
	void UpdateRoomAIState(FName RoomId, FVector AILocation, EEntityState AIState);

	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<FClockbaitRoomAIState> RoomAIStates;

	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 ReadyCount{};
};
