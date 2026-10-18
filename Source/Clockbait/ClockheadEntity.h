// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ClockbaitGameState.h"
#include "ClockheadEntity.generated.h"

class AClockbaitCharacter;

UCLASS()
class CLOCKBAIT_API AClockheadEntity : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AClockheadEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	EEntityState OwnState = EEntityState::Patrolling;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ChaseSpeed{ 600.0f };

	UPROPERTY(EditAnywhere, Category = "Movement")
	float PatrolSpeed{ 350.0f };

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AlarmSpeed{ 500.0f };

	UPROPERTY(EditAnywhere)
	float KillOffsetDistance = 205.0f;

	FTimerHandle EntityFreezeTimer;

	//Guards StartKill: the BT keeps ticking CatchPlayer, so without this the kill restarts itself
	bool bKillInProgress = false;

	UPROPERTY(EditAnywhere, Category = "Kill")
	class UAnimMontage* KillMontage = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetEntityState(EEntityState StateToSet);

	EEntityState GetEntityState() const { return OwnState; }

	void Freeze(float Duration);

	void StartKill(AClockbaitCharacter* Victim);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayKillMontage();

	void Unfreeze();
};
