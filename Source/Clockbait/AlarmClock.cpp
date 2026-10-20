// Fill out your copyright notice in the Description page of Project Settings.


#include "AlarmClock.h"
#include "ClockbaitRoomVolume.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ClockheadEntity.h"
#include "Clockbait.h"
#include "NavigationSystem.h"

// Sets default values
AAlarmClock::AAlarmClock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	NavTarget = CreateDefaultSubobject<USceneComponent>(TEXT("NavTarget"));
	NavTarget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAlarmClock::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAlarmClock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAlarmClock::TryActivate()
{
	if (bOnCooldown || !AssignedEnemy) {
		return;
	}
	if (AssignedEnemy->GetEntityState() == EEntityState::Frozen) return;
	if (!OwningRoom) {
		UE_LOG(LogClockbait, Warning, TEXT("No Room assigned to Alarm"));
		return;
	}
	if (!OwningRoom->AreAllAlarmsOff()) return;
	bOnCooldown = true;
	float Dist = GetDistanceTo(AssignedEnemy);
	if (Dist > EnemyProximityThreshold) {
		ActivateAlarm();
	}
	else {
		DeactivateAlarm();
	}

}

void AAlarmClock::ActivateAlarm() {
	bRinging = true;
	MulticastPlayRingingSound();
	if (AssignedEnemy) {
		UE_LOG(LogTemp, Display, TEXT("AssignedEnemy set correctly"));
		if (AAIController* AIController = Cast<AAIController>(AssignedEnemy->GetController())) {
			UE_LOG(LogTemp, Display, TEXT("AIController set correctly"));
			if (UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent()) {
				UE_LOG(LogTemp, Display, TEXT("Blackboard set correctly"));
				if (NavTarget) {
					if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())) {
						FNavLocation NavLoc;
						if (NavSys->ProjectPointToNavigation(NavTarget->GetComponentLocation(), NavLoc, FVector(10.0f, 10.0f, 200.0f))) {
							UE_LOG(LogTemp, Display, TEXT("Alarm Values set correctly"));
							Blackboard->SetValueAsVector(TEXT("AlarmLocation"), NavLoc.Location);
							Blackboard->SetValueAsObject(TEXT("Alarm"), this);
						}
						else {
							UE_LOG(LogTemp, Warning, TEXT("Alarm Values cannot be set"));
						}
					}
				}
			}
		}
	}
}

void AAlarmClock::DeactivateAlarm() {
	bRinging = false;
	if (AssignedEnemy)
	{
		if (AAIController* AIController = Cast<AAIController>(AssignedEnemy->GetController())) {
			if (UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent()) {
				//Only clear the key if it is still ours, so silencing one alarm can't cancel another
				if (Blackboard->GetValueAsObject(TEXT("Alarm")) == this) {
					Blackboard->ClearValue(TEXT("AlarmLocation"));
					Blackboard->ClearValue(TEXT("Alarm"));
				}
			}
		}
	}
	if (!CooldownTimerHandle.IsValid()) {
		UE_LOG(LogTemp, Display, TEXT("On CooldownExpired timer stared"));
		GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &AAlarmClock::OnCooldownExpired, CooldownDuration, false);
	}
	MulticastStopRingingSound();
}

void AAlarmClock::OnCooldownExpired()
{
	GetWorldTimerManager().ClearTimer(CooldownTimerHandle);
	bOnCooldown = false;
}

void AAlarmClock::Interact_Implementation(APawn* Interactor)
{
	TryActivate();
}

