// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HidingSpot.generated.h"

/**
 *  A single-occupant hiding spot (locker, desk, ...). Entering one marks the occupying player
 *  as bIsHiddenFromAI so BTService_UpdatePlayerLocation stops perceiving them.
 */
UCLASS()
class CLOCKBAIT_API AHidingSpot : public AActor
{
	GENERATED_BODY()

public:
	AHidingSpot();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Hiding")
	void ServerEnterHiding(APawn* RequestingPawn);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Hiding")
	void ServerExitHiding(APawn* RequestingPawn);

	UFUNCTION(BlueprintCallable, Category = "Hiding")
	bool IsOccupied() const { return OccupyingPawn != nullptr; }

protected:
	/** Implemented in Blueprint: purely cosmetic (animation, camera) - the actual hidden/perception state is already set in C++ by the time this fires. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Hiding")
	void OnServerEnterHiding(APawn* RequestingPawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "Hiding")
	void OnServerExitHiding(APawn* RequestingPawn);

	/** Implemented in Blueprint: runs on every client, for reacting to occupancy changes (e.g. marking the spot as taken) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Hiding")
	void OnHidingOccupancyChanged(APawn* NewOccupant);

	UFUNCTION()
	void OnRep_OccupyingPawn();

	UPROPERTY(ReplicatedUsing = OnRep_OccupyingPawn)
	APawn* OccupyingPawn = nullptr;
};
