// Fill out your copyright notice in the Description page of Project Settings.


#include "ClockheadEntity.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "ClockbaitCharacter.h"
#include "Camera/CameraComponent.h"
#include "BrainComponent.h"
#include "Clockbait.h"

// Sets default values
AClockheadEntity::AClockheadEntity()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AClockheadEntity::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
}

// Called every frame
void AClockheadEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClockheadEntity::SetEntityState(EEntityState StateToSet)
{
	if (OwnState == StateToSet) return;
	OwnState = StateToSet;
	if (OwnState == EEntityState::Alarm) {
		GetCharacterMovement()->MaxWalkSpeed = AlarmSpeed;
	}
	else if (OwnState == EEntityState::Patrolling) {
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	}
}

void AClockheadEntity::Freeze(float Duration)
{
	if (AAIController* AIC = Cast<AAIController>(GetController())) {
		UBrainComponent* Brain = AIC->GetBrainComponent();
		if (!Brain) return;
		Brain->StopLogic("The puzzle was solved");
		AIC->StopMovement();
		GetWorld()->GetTimerManager().SetTimer(
			EntityFreezeTimer,
			this,
			&AClockheadEntity::Unfreeze,
			Duration
		);
	}

}

void AClockheadEntity::StartKill(AClockbaitCharacter* Victim)
{
	if (!Victim) return;
	if (bKillInProgress) return;
	bKillInProgress = true;

	Freeze(10.0f);
	//StopMovement only ends path following - braking would still carry the entity forwards
	//for several frames and eat the offset we are about to set.
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	FVector Dir = (Victim->GetActorLocation() - GetActorLocation());
	Dir.Z = 0.0f;
	if (!Dir.Normalize()) { return; }

	SetActorRotation(Dir.Rotation());
	FVector KillLocation = Victim->GetActorLocation() - Dir * KillOffsetDistance;
	KillLocation.Z = GetActorLocation().Z;
	SetActorLocation(KillLocation, true);

	MulticastPlayKillMontage();

	UE_LOG(LogClockbait, Display, TEXT("StartKill: distance to '%s' after repositioning is %f (wanted %f)"),
		*GetNameSafe(Victim), FVector::Dist2D(GetActorLocation(), Victim->GetActorLocation()), KillOffsetDistance);

	Victim->ClientPlayKillCam(this, (-Dir).Rotation());
}

void AClockheadEntity::MulticastPlayKillMontage_Implementation()
{
	PlayAnimMontage(KillMontage);
}

void AClockheadEntity::Unfreeze()
{
	if (AAIController* AIC = Cast<AAIController>(GetController())) {
		UBrainComponent* Brain = AIC->GetBrainComponent();
		if (!Brain) return;
		Brain->RestartLogic();
	}
}



