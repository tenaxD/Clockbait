// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleController.h"
#include "PuzzleStation.h"
#include "ColorPuzzleStation.h"
#include "ClockbaitRoomVolume.h"
#include "ClockbaitRoomVolume.h"
#include "Clockbait.h"

// Sets default values
APuzzleController::APuzzleController()
{
	 // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void APuzzleController::NotifyStationSolved(APuzzleStation* Station)
{
	if (!HasAuthority() || bSegmentComplete) {
		return;
	}

	for (auto i = 0; i < Stations.Num(); ++i) {
		// An empty slot blocks the segment instead of being skipped - BeginPlay already warned about
		// it, and opening the doors without it would be finishing the puzzle by accident.
		if (!Stations[i] || !Stations[i]->IsSolved()) {
			UE_LOG(LogClockbait, Display, TEXT("NotifyStationSolved: blocked by Stations slot %d ('%s')"), i, *GetNameSafe(Stations[i]));
			return;
		}
	}

	bSegmentComplete = true;

	for (auto i = 0; i < Stations.Num(); ++i) {
		if (AClockbaitRoomVolume* Room = Stations[i]->GetRoom()) {
			Room->MarkPuzzleSolved();
		}
		else {
			UE_LOG(LogClockbait, Warning, TEXT("NotifyStationSolved: station '%s' has no Room"), *GetNameSafe(Stations[i]));
		}
	}
}

void APuzzleController::NotifyStationFailed(APuzzleStation* Station)
{
	if (!HasAuthority() || bSegmentComplete) {
		return;
	}

	// Shared attempt counter, failure sound and the AI speed-up in both rooms belong here.
}

void APuzzleController::NotifyRoomVacated(AClockbaitRoomVolume* Room)
{
	if (!HasAuthority()) return;
	bool bSeal = true;
	for (auto i = 0; i < Stations.Num(); ++i) {
		if (!Stations[i]) continue;
		if (AClockbaitRoomVolume* RoomToCheck = Stations[i]->GetRoom()) {
			if (RoomToCheck->HasPlayerInside()) {
				UE_LOG(LogClockbait, Display, TEXT("There is a player in a room: %s"), *GetNameSafe(RoomToCheck));
				bSeal = false;
			} 
		}
		else {
			UE_LOG(LogClockbait, Warning, TEXT("NotifyRoomVacated: station '%s' has no Room"), *GetNameSafe(Stations[i]));
		}
	}
	if (bSeal) {
		for (auto Station : Stations) {
			if (!Station) continue;
			if (!Station->GetRoom()) continue;
			Station->GetRoom()->Seal();
		}
	}
	
}

// Called when the game starts or when spawned
void APuzzleController::BeginPlay()
{
	Super::BeginPlay();
	if (Stations.Num() < 1) {
		UE_LOG(LogClockbait, Warning, TEXT("PuzzleController '%s': Stations is empty, no station will ever report in"), *GetName());
	}

	for (auto i = 0; i < Stations.Num(); ++i) {
		if (!Stations[i]) {
			UE_LOG(LogClockbait, Warning, TEXT("PuzzleController '%s': Stations slot %d is empty"), *GetName(), i);
			continue;
		}

		Stations[i]->SetPuzzleController(this);
		if (AClockbaitRoomVolume* Room = Stations[i]->GetRoom()) {
			Room->SetPuzzleController(this);
		}
		if (AColorPuzzleStation* ColorStation = Cast<AColorPuzzleStation>(Stations[i])) {
			ColorStation->SetSolution(FinalSolution);
		}
	}
}

// Called every frame
void APuzzleController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
