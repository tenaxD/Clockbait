// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClockbaitRoomVolume.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/Pawn.h"
#include "ClockbaitCharacter.h"
#include "AutomaticDoor.h"

AClockbaitRoomVolume::AClockbaitRoomVolume()
{
}

void AClockbaitRoomVolume::SetExitLocked(bool bNewLocked)
{
	if (ExitDoor)
	{
		ExitDoor->SetDoorLocked(bNewLocked);
	}
}

void AClockbaitRoomVolume::MarkPuzzleSolved()
{
	SetExitLocked(false);
	bPuzzleSolved = true;
}

bool AClockbaitRoomVolume::IsPawnInRoom(APawn* Pawn) const
{
	const UShapeComponent* Collision = GetCollisionComponent();
	return Pawn && Collision && Collision->IsOverlappingActor(Pawn);
}

void AClockbaitRoomVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(OtherActor)) {
		if (HasAuthority() && !bPuzzleSolved) {
			if (EntranceDoor && ExitDoor) {
				EntranceDoor->SetDoorLocked(true);
				ExitDoor->SetDoorLocked(true);
			}
		}
		if (bIsRoomDark) {
			Player->ToggleFlashlight(true);
		}
	}
}

void AClockbaitRoomVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(OtherActor)) {
		if (HasAuthority()) {
			if (EntranceDoor && ExitDoor) {
				if (!bPuzzleSolved) {
					EntranceDoor->SetDoorLocked(false);
				}
				ExitDoor->SetDoorLocked(false);
			}
		}

		Player->ToggleFlashlight(false);
	}
}
