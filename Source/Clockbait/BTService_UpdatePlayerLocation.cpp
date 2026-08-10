// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_UpdatePlayerLocation.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "ClockbaitRoomVolume.h"
#include "ClockbaitCharacter.h"
#include "ClockbaitGameState.h"
#include "Clockbait.h"

namespace
{
	const FName IsPlayerSeenKey(TEXT("IsPlayerSeen"));
	const FName PlayerLocationKey(TEXT("PlayerLocation"));
	const FName LastKnownPlayerLocationKey(TEXT("LastKnownPlayerLocation"));
	const FName TargetActorKey(TEXT("TargetActor"));
	const FName RoomVolumeKey(TEXT("RoomVolume"));
}

UBTService_UpdatePlayerLocation::UBTService_UpdatePlayerLocation()
{
	NodeName = TEXT("Update Player Location");
}

uint16 UBTService_UpdatePlayerLocation::GetInstanceMemorySize() const
{
	return sizeof(FBTUpdatePlayerLocationMemory);
}

void UBTService_UpdatePlayerLocation::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FBTUpdatePlayerLocationMemory>(NodeMemory, InitType);
}

void UBTService_UpdatePlayerLocation::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTUpdatePlayerLocationMemory>(NodeMemory, CleanupType);
}

void UBTService_UpdatePlayerLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!AIController || !Blackboard)
	{
		return;
	}

	UAIPerceptionComponent* PerceptionComponent = AIController->GetPerceptionComponent();
	APawn* AIPawn = AIController->GetPawn();

	if (!PerceptionComponent || !AIPawn)
	{
		return;
	}

	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(nullptr, PerceivedActors);

	FBTUpdatePlayerLocationMemory* Memory = CastInstanceNodeMemory<FBTUpdatePlayerLocationMemory>(NodeMemory);

	// RoomVolume lives on the Blackboard (set once per AI instance in BP), not as a UPROPERTY on this
	// node - this node/asset is shared by every AI using BT_EnemyAI, so a UPROPERTY here would be one
	// value shared by all of them instead of one per room.
	AClockbaitRoomVolume* RoomVolume = Cast<AClockbaitRoomVolume>(Blackboard->GetValueAsObject(RoomVolumeKey));
	if (!RoomVolume && !Memory->bLoggedMissingRoomVolume)
	{
		Memory->bLoggedMissingRoomVolume = true;
		UE_LOG(LogClockbait, Warning, TEXT("UBTService_UpdatePlayerLocation: '%s' has no RoomVolume Blackboard key set; falling back to tracking players across the whole level."), *GetNameSafe(AIController));
	}

	const FVector AILocation = AIPawn->GetActorLocation();
	AActor* BestTarget = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (AActor* Actor : PerceivedActors)
	{
		APawn* Pawn = Cast<APawn>(Actor);
		if (!Pawn || !Pawn->IsPlayerControlled())
		{
			continue;
		}

		if (RoomVolume && !RoomVolume->IsPawnInRoom(Pawn))
		{
			continue;
		}

		if (const AClockbaitCharacter* Character = Cast<AClockbaitCharacter>(Pawn))
		{
			if (Character->IsHiddenFromAI())
			{
				continue;
			}
		}

		const float DistSq = FVector::DistSquared(AILocation, Actor->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestTarget = Actor;
		}
	}

	if (BestTarget)
	{
		Memory->LockedTarget = BestTarget;
		Memory->TimeSinceLostSight = 0.0f;
	}
	else if (Memory->LockedTarget.IsValid())
	{
		// Perception dropped. Hold onto the previous target for a moment so a player who runs
		// behind the AI, or breaks line of sight briefly, doesn't shake it off instantly.
		Memory->TimeSinceLostSight += DeltaSeconds;

		if (Memory->TimeSinceLostSight <= LoseTargetDelay)
		{
			BestTarget = Memory->LockedTarget.Get();
		}
		else
		{
			Memory->LockedTarget.Reset();
		}
	}

	if (BestTarget)
	{
		FVector TargetLocation = BestTarget->GetActorLocation();
		FVector ProjectedLocation;
		if (ProjectToNavMesh(*AIController, TargetLocation, ProjectedLocation))
		{
			TargetLocation = ProjectedLocation;
		}
		else
		{
			UE_LOG(LogClockbait, Verbose, TEXT("UBTService_UpdatePlayerLocation: Could not project '%s' location onto the NavMesh; using raw actor location."), *GetNameSafe(BestTarget));
		}

		Blackboard->SetValueAsBool(IsPlayerSeenKey, true);
		Blackboard->SetValueAsVector(PlayerLocationKey, TargetLocation);
		Blackboard->SetValueAsVector(LastKnownPlayerLocationKey, TargetLocation);
		Blackboard->SetValueAsObject(TargetActorKey, BestTarget);
	}
	else
	{
		Blackboard->SetValueAsBool(IsPlayerSeenKey, false);
		Blackboard->ClearValue(PlayerLocationKey);
		Blackboard->ClearValue(LastKnownPlayerLocationKey);
		Blackboard->ClearValue(TargetActorKey);
	}

	// Publish this room's AI position to the informant's minimap. Only possible once RoomVolume is
	// configured, since that's what tells us which room's entry to update in GameState.
	if (RoomVolume)
	{
		if (AClockbaitGameState* GameState = AIController->GetWorld()->GetGameState<AClockbaitGameState>())
		{
			GameState->UpdateRoomAIState(RoomVolume->RoomId, AILocation, BestTarget != nullptr);
		}
	}
}

bool UBTService_UpdatePlayerLocation::ProjectToNavMesh(const AAIController& AIController, const FVector& Location, FVector& OutProjectedLocation) const
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIController.GetWorld());
	if (!NavSys)
	{
		return false;
	}

	FNavLocation NavLocation;
	if (!NavSys->ProjectPointToNavigation(Location, NavLocation, NavProjectionExtent, &AIController.GetNavAgentPropertiesRef()))
	{
		return false;
	}

	OutProjectedLocation = NavLocation.Location;
	return true;
}
