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

	UPROPERTY(EditAnywhere, Category = "Room")
	bool bIsRoomDark = false;

	UPROPERTY(EditAnywhere, Category = "Room")
	class AAutomaticDoor* EntranceDoor{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Room")
	class AAutomaticDoor* ExitDoor{ nullptr };

	UPROPERTY()
	class APuzzleController* PuzzleController{ nullptr };

	UPROPERTY()
	bool bPuzzleSolved{ false };

	//The room gets sealed meaning there is no coming back once both players exit after completing the puzzle
	UPROPERTY()
	bool bSealed{ false };

	/**
	 * The room is the only thing that holds pointers to its doors - everything else (the puzzle
	 * station now, the puzzle controller later) asks the room instead of keeping its own copy.
	 * One owner means there is nothing to keep in sync, and a spawner building rooms at runtime
	 * has one link to wire instead of three.
	 */
	UFUNCTION(BlueprintCallable, Category = "Room")
	void SetExitLocked(bool bNewLocked);
	
	void SetPuzzleController(class APuzzleController* ControllerRef) { PuzzleController = ControllerRef; }

	UFUNCTION(BlueprintCallable, Category = "Room")
	void MarkPuzzleSolved();


	void Seal();

	void GetRoomBounds(FVector& OutOrigin, FVector& OutExtent) const;

	/**
	 * True if Pawn is currently overlapping this room's volume. Queried live off the collision
	 * component (not a cached Begin/EndOverlap list) so it's correct even for a pawn that spawned
	 * already inside the volume, which never fires a BeginOverlap event.
	 */
	UFUNCTION(BlueprintCallable, Category = "Room")
	bool IsPawnInRoom(APawn* Pawn) const;

	UFUNCTION(BlueprintCallable, Category = "Room")
	bool HasPlayerInside() const;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

};
