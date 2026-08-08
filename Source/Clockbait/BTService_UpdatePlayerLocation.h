// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdatePlayerLocation.generated.h"

/** Per-instance state: the player we last locked onto, and how long perception has been dropped */
struct FBTUpdatePlayerLocationMemory
{
	TWeakObjectPtr<AActor> LockedTarget;
	float TimeSinceLostSight = 0.0f;
};

/**
 * Keeps PlayerLocation / LastKnownPlayerLocation / IsPlayerSeen in sync with
 * whichever player-controlled pawn is currently nearest among those perceived,
 * instead of always tracking a hardcoded player index.
 *
 * Once a player has been perceived the AI stays locked onto them for LoseTargetDelay
 * seconds after perception drops, so slipping behind the AI for a moment is not enough
 * to shake it off.
 */
UCLASS()
class CLOCKBAIT_API UBTService_UpdatePlayerLocation : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_UpdatePlayerLocation();

	virtual uint16 GetInstanceMemorySize() const override;
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/** How long the AI keeps chasing a player it can no longer perceive before giving up on them */
	UPROPERTY(EditAnywhere, Category = "Perception", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float LoseTargetDelay = 3.0f;
};
