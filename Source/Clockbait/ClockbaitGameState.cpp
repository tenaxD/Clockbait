// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClockbaitGameState.h"
#include "Net/UnrealNetwork.h"

void AClockbaitGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AClockbaitGameState, RoomAIStates);
	DOREPLIFETIME(AClockbaitGameState, ReadyCount);
}

void AClockbaitGameState::UpdateRoomAIState(FName RoomId, FVector AILocation, EEntityState AIState)
{
	if (!HasAuthority())
	{
		return;
	}

	// Only ever 2 rooms, so a linear find is simpler than anything fancier and costs nothing.
	FClockbaitRoomAIState* Existing = RoomAIStates.FindByPredicate([RoomId](const FClockbaitRoomAIState& State)
	{
		return State.RoomId == RoomId;
	});

	if (!Existing)
	{
		Existing = &RoomAIStates.AddDefaulted_GetRef();
		Existing->RoomId = RoomId;
	}

	Existing->AILocation = AILocation;
	Existing->EnemyState = AIState;
}

