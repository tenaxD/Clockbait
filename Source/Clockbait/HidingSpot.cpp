// Copyright Epic Games, Inc. All Rights Reserved.

#include "HidingSpot.h"
#include "ClockbaitCharacter.h"
#include "Net/UnrealNetwork.h"

AHidingSpot::AHidingSpot()
{
	bReplicates = true;
}

void AHidingSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHidingSpot, OccupyingPawn);
}

void AHidingSpot::ServerEnterHiding_Implementation(APawn* RequestingPawn)
{
	// Reject rather than queue/replace - occupancy conflicts shouldn't happen in practice since the
	// two players are always in different rooms, so a hard no-op here is enough.
	if (!RequestingPawn || OccupyingPawn != nullptr)
	{
		return;
	}

	OccupyingPawn = RequestingPawn;

	// Set directly in C++ (not via the BlueprintImplementableEvent below) because AI perception
	// correctness depends on it - this must always happen, not be left to whatever the BP graph does.
	if (AClockbaitCharacter* Character = Cast<AClockbaitCharacter>(RequestingPawn))
	{
		Character->SetHiddenFromAI(true);
	}

	OnRep_OccupyingPawn(); // OnRep only fires on clients; call manually so the server also gets OnHidingOccupancyChanged
	OnServerEnterHiding(RequestingPawn);
}

void AHidingSpot::ServerExitHiding_Implementation(APawn* RequestingPawn)
{
	if (RequestingPawn != OccupyingPawn)
	{
		return;
	}

	OccupyingPawn = nullptr;

	if (AClockbaitCharacter* Character = Cast<AClockbaitCharacter>(RequestingPawn))
	{
		Character->SetHiddenFromAI(false);
	}

	OnRep_OccupyingPawn();
	OnServerExitHiding(RequestingPawn);
}

void AHidingSpot::OnRep_OccupyingPawn()
{
	OnHidingOccupancyChanged(OccupyingPawn);
}
