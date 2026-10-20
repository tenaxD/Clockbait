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

	UPROPERTY(EditAnywhere, Category = "Room", ReplicatedUsing = OnRep_IsRoomDark)
	bool bIsRoomDark = false;

	UPROPERTY(EditAnywhere, Category = "Room")
	class AClockheadEntity* BoundEntity{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Room")
	class AAutomaticDoor* EntranceDoor{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Room")
	class AAutomaticDoor* ExitDoor{ nullptr };

	UPROPERTY()
	class APuzzleController* PuzzleController{ nullptr };

	UPROPERTY()
	class AClockbaitCharacter* CompromisedPlayer{ nullptr };

	UPROPERTY(EditInstanceOnly, Category = "Room")
	TArray<class AAlarmClock*> RoomAlarms;

	UPROPERTY(EditInstanceOnly, Category = "Room")
	TArray<class AHidingSpot*> RoomHidingSpots;

	UPROPERTY(EditInstanceOnly, Category = "Room")
	TArray<AActor*> RoomIndicators;

	UPROPERTY(ReplicatedUsing = OnRep_PuzzleSolved)
	bool bPuzzleSolved{ false };

	//This room's own station, not the segment - the other room may still be unsolved
	UPROPERTY(ReplicatedUsing = OnRep_RefreshIndicators)
	bool bStationSolved{ false };

	UPROPERTY(ReplicatedUsing = OnRep_RefreshIndicators)
	int32 TriesCount{ -1 };

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

	bool AreAllAlarmsOff();

	void SetStationSolved();

	void SetTriesCount(int32 NewTriesCount);
protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UFUNCTION()
	void OnRep_IsRoomDark();

	UFUNCTION()
	void OnRep_RefreshIndicators();

	UFUNCTION()
	void OnRep_PuzzleSolved();

	UPROPERTY(EditInstanceOnly, Category = "Room")
	TArray<AActor*> RoomLights;

	UPROPERTY(EditDefaultsOnly, Category = "Indicators")
	int32 IndicatorGlassSlot{ 0 };

	UPROPERTY(EditDefaultsOnly, Category = "Indicators")
	FLinearColor LampColorGreen{ 0.022, 0.661, 0.0 };

	UPROPERTY(EditDefaultsOnly, Category = "Indicators")
	FLinearColor LampColorRed{ 0.865, 0.0, 0.012 };

	

	UPROPERTY(EditAnywhere)
	float EntityFreezeTime{ 20.0f };

	UPROPERTY(EditInstanceOnly, Category = "Room")
	class USoundBase* LightsOnSound = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "Room")
	class USoundBase* PuzzleSolvedSound = nullptr;

	//Needs blueprint. Will be used for toggling lights, playing sounds etc.
	/*UFUNCTION(BlueprintImplementableEvent, Category = "Room")
	void OnRoomDarkChanged(bool bNewDark);*/

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void GetPlayersInside(TArray<AClockbaitCharacter*>& OutPlayers) const;

};
