// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClockbaitRoomVolume.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/Pawn.h"

AClockbaitRoomVolume::AClockbaitRoomVolume()
{
}

bool AClockbaitRoomVolume::IsPawnInRoom(APawn* Pawn) const
{
	const UShapeComponent* Collision = GetCollisionComponent();
	return Pawn && Collision && Collision->IsOverlappingActor(Pawn);
}
