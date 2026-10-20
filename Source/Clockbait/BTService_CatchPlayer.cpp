// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CatchPlayer.h"
#include "AIController.h"
#include "Clockbait.h"
#include "ClockheadEntity.h"
#include "ClockbaitGameState.h"
#include "ClockbaitGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClockbaitCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

namespace {
	const FName CatchTargetActorKey(TEXT("TargetActor"));
}

UBTService_CatchPlayer::UBTService_CatchPlayer()
{
	NodeName = TEXT("Catch Player");
}

uint16 UBTService_CatchPlayer::GetInstanceMemorySize() const
{
	return sizeof(FBTCatchPlayerMemory);
}

void UBTService_CatchPlayer::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FBTCatchPlayerMemory>(NodeMemory, InitType);
}

void UBTService_CatchPlayer::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTCatchPlayerMemory>(NodeMemory, CleanupType);
}

void UBTService_CatchPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	FBTCatchPlayerMemory* Memory = CastInstanceNodeMemory<FBTCatchPlayerMemory>(NodeMemory);

	if (!AIController || !Blackboard)
	{
		return;
	}
	APawn* AIPawn = AIController->GetPawn();
	AClockheadEntity* Entity = Cast<AClockheadEntity>(AIPawn);
	if (!Entity) return;
	if (Entity->GetEntityState() != EEntityState::Chasing && Entity->GetEntityState() != EEntityState::Alarm) return;
	

	APawn* Target = Cast<APawn>(Blackboard->GetValueAsObject(CatchTargetActorKey));
	if (!Target) return;

	float DistSq = FVector::DistSquared2D(AIPawn->GetActorLocation(), Target->GetActorLocation());
	/*UE_LOG(LogClockbait, Display, TEXT("Distance squared: %f"), DistSq);*/

	if (Entity->GetEntityState() == EEntityState::Chasing) {
		AClockbaitCharacter* TargetChar = Cast<AClockbaitCharacter>(Target);
		const bool bHidden = TargetChar && TargetChar->CurrentHidingSpot != nullptr;
		const float EffectiveRadius = bHidden ? HiddenCatchRadius : CatchRadius;
		if (DistSq <= FMath::Square(EffectiveRadius)) {
			UE_LOG(LogClockbait, Display, TEXT("Player Caught"));
			if (AClockbaitGameMode* GM = AIController->GetWorld()->GetAuthGameMode<AClockbaitGameMode>())
			{
				AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(Target);
				if (!Player) return;
				Entity->StartKill(Player);
				GM->NotifyPlayerCaught(Target);
			}
		}
	}
	else if (Entity->GetEntityState() == EEntityState::Alarm) {
		const float Intended = Entity->GetCharacterMovement()->MaxWalkSpeed;
		if (Memory->BlockageDuration >= BlockageDeathThreshold) {
			AClockbaitGameMode* GM = AIController->GetWorld()->GetAuthGameMode<AClockbaitGameMode>();
			if (!GM) return;
			AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(Target);
			if (!Player) return;
			Entity->StartKill(Player);
			GM->NotifyPlayerCaught(Target);
			Memory->BlockageDuration = 0.0f;
		}

		if (AIPawn->GetVelocity().SizeSquared2D() < FMath::Square(Intended * BlockedSpeedFraction)) {
			Memory->BlockageDuration += DeltaSeconds;
		}
		else {
			Memory->BlockageDuration = 0.0f;
		}
	}
}
