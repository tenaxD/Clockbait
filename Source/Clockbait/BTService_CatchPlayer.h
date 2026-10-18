// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CatchPlayer.generated.h"

struct FBTCatchPlayerMemory {
	float BlockageDuration{};
};

/**
 * 
 */
UCLASS()
class CLOCKBAIT_API UBTService_CatchPlayer : public UBTService
{
	GENERATED_BODY()
protected:
	UBTService_CatchPlayer();

	virtual uint16 GetInstanceMemorySize() const override;
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	float CatchRadius{ 150.0f };

	UPROPERTY(EditAnywhere)
	float HiddenCatchRadius{ 180.0f };

	UPROPERTY(EditAnywhere)
	float BlockageDeathThreshold{1.0f};

	UPROPERTY(EditAnywhere)
	float BlockedSpeedFraction{ 0.2f };
};
