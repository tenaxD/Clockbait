// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClockbaitRoomVolume.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/Pawn.h"
#include "ClockbaitCharacter.h"

AClockbaitRoomVolume::AClockbaitRoomVolume()
{
}

bool AClockbaitRoomVolume::IsPawnInRoom(APawn* Pawn) const
{
	const UShapeComponent* Collision = GetCollisionComponent();
	return Pawn && Collision && Collision->IsOverlappingActor(Pawn);
}

void AClockbaitRoomVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (bIsRoomDark) {
		if (AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(OtherActor)) {
			Player->ToggleFlashlight(true);
		}
	}
}

void AClockbaitRoomVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(OtherActor)) {
		Player->ToggleFlashlight(false);
	}
}
