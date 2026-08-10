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

	// Set once a "RoomVolume Blackboard key not configured" warning has been logged for this AI instance,
	// so the fallback-to-global-tracking case doesn't spam the log every tick.
	bool bLoggedMissingRoomVolume = false;
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

	/** Search extent used to snap a tracked player location onto the NavMesh, so LastKnownPlayerLocation is never an unreachable off-mesh point (e.g. a doorway threshold) */
	UPROPERTY(EditAnywhere, Category = "Perception")
	FVector NavProjectionExtent = FVector(200.0f, 200.0f, 500.0f);

private:
	/** Projects Location onto the NavMesh using AIController's nav agent. Returns false (leaving OutProjectedLocation untouched) if no nearby NavMesh point was found. */
	bool ProjectToNavMesh(const AAIController& AIController, const FVector& Location, FVector& OutProjectedLocation) const;
};
