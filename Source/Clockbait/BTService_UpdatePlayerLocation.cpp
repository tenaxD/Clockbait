// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_UpdatePlayerLocation.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

namespace
{
	const FName IsPlayerSeenKey(TEXT("IsPlayerSeen"));
	const FName PlayerLocationKey(TEXT("PlayerLocation"));
	const FName LastKnownPlayerLocationKey(TEXT("LastKnownPlayerLocation"));
	const FName TargetActorKey(TEXT("TargetActor"));
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

	const FVector AILocation = AIPawn->GetActorLocation();
	AActor* BestTarget = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (AActor* Actor : PerceivedActors)
	{
		const APawn* Pawn = Cast<APawn>(Actor);
		if (!Pawn || !Pawn->IsPlayerControlled())
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(AILocation, Actor->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestTarget = Actor;
		}
	}

	FBTUpdatePlayerLocationMemory* Memory = CastInstanceNodeMemory<FBTUpdatePlayerLocationMemory>(NodeMemory);

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
		const FVector TargetLocation = BestTarget->GetActorLocation();
		Blackboard->SetValueAsBool(IsPlayerSeenKey, true);
		Blackboard->SetValueAsVector(PlayerLocationKey, TargetLocation);
		Blackboard->SetValueAsVector(LastKnownPlayerLocationKey, TargetLocation);
		Blackboard->SetValueAsObject(TargetActorKey, BestTarget);
	}
	else
	{
		Blackboard->SetValueAsBool(IsPlayerSeenKey, false);
		Blackboard->ClearValue(PlayerLocationKey);
		Blackboard->ClearValue(TargetActorKey);
	}
}
