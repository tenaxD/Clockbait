// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "ClockbaitRoomVolume.generated.h"

/**
 *  Marks out one room's floor area. Each room's AI is configured (in its Blackboard) to
 *  reference one of these, so BTService_UpdatePlayerLocation only tracks players standing
 *  inside that specific room instead of scanning the whole level.
 */
UCLASS()
class CLOCKBAIT_API AClockbaitRoomVolume : public ATriggerBox
{
	GENERATED_BODY()

public:
	AClockbaitRoomVolume();

	/** Identifies which room this is (e.g. "RoomA"). Set per-instance in the editor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
	FName RoomId;

	/**
	 * True if Pawn is currently overlapping this room's volume. Queried live off the collision
	 * component (not a cached Begin/EndOverlap list) so it's correct even for a pawn that spawned
	 * already inside the volume, which never fires a BeginOverlap event.
	 */
	UFUNCTION(BlueprintCallable, Category = "Room")
	bool IsPawnInRoom(APawn* Pawn) const;
};
