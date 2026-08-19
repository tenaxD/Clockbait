// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdatePlayerLocation.generated.h"

/** Per-instance state: where the target was, and how they were moving, the last time we truly saw them */
struct FBTUpdatePlayerLocationMemory
{
	// Set once a "RoomVolume Blackboard key not configured" warning has been logged for this AI instance,
	// so the fallback-to-global-tracking case doesn't spam the log every tick.
	bool bLoggedMissingRoomVolume = false;

	FVector LastPerceivedLocation = FVector::ZeroVector;

	FVector LastPerceivedVelocity = FVector::ZeroVector;
};

/**
 * Keeps PlayerLocation / LastKnownPlayerLocation / IsPlayerSeen in sync with
 * whichever player-controlled pawn is currently nearest among those perceived,
 * instead of always tracking a hardcoded player index.
 *
 * IsPlayerSeen / PlayerLocation / TargetActor all mean "right now": they are cleared in the same
 * frame sight is lost, which is what stops Move To -> Target Actor from homing in on a player the
 * AI can no longer see. LastKnownPlayerLocation is a separate, frozen trail left behind for the
 * BT's search branch, extrapolated a little along the player's heading so the AI rounds the corner
 * they disappeared behind instead of stopping at it. The BT owns clearing that key, once the AI
 * has been there and found nothing.
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

	/** Search extent used to snap a tracked player location onto the NavMesh, so LastKnownPlayerLocation is never an unreachable off-mesh point (e.g. a doorway threshold) */
	UPROPERTY(EditAnywhere, Category = "Perception")
	FVector NavProjectionExtent = FVector(200.0f, 200.0f, 500.0f);

	/** How far ahead of the last seen position AI guesses the player kept running */
	UPROPERTY(EditAnywhere, Category = "Perception", meta = (ClampMin = "0.0"))
	float SearchLeadDistance = 400.0f;

private:
	/** Projects Location onto the NavMesh using AIController's nav agent. Returns false (leaving OutProjectedLocation untouched) if no nearby NavMesh point was found. */
	bool ProjectToNavMesh(const AAIController& AIController, const FVector& Location, FVector& OutProjectedLocation) const;
};
