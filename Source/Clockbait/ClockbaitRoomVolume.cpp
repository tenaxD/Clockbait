// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClockbaitRoomVolume.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/Pawn.h"
#include "ClockbaitCharacter.h"
#include "PuzzleController.h"
#include "AutomaticDoor.h"
#include "Net/UnrealNetwork.h"
#include "Clockbait.h"
#include "ClockheadEntity.h"
#include "AlarmClock.h"
#include "ClockbaitPlayerController.h"

AClockbaitRoomVolume::AClockbaitRoomVolume()
{
	bReplicates = true;
}

void AClockbaitRoomVolume::BeginPlay()
{
	Super::BeginPlay();
	for (AAlarmClock* Alarm : RoomAlarms)
	{
		if (Alarm) { Alarm->SetOwningRoom(this); }
	}
}

bool AClockbaitRoomVolume::AreAllAlarmsOff()
{
	for (AAlarmClock* Alarm : RoomAlarms)
	{
		if (!Alarm) continue;
		if (Alarm->IsRinging()) {
			UE_LOG(LogClockbait, Display, TEXT("Cannot Turn Alarm on because an alarm is ringing"));
			return false;
		}
	}
	return true;
}

void AClockbaitRoomVolume::SetExitLocked(bool bNewLocked)
{
	if (ExitDoor)
	{
		ExitDoor->SetDoorLocked(bNewLocked);
	}
}

void AClockbaitRoomVolume::MarkPuzzleSolved()
{
	SetExitLocked(false);
	bPuzzleSolved = true;
	if (bIsRoomDark) {
		bIsRoomDark = false;
		OnRep_IsRoomDark();
	}
	//Deactivating all alarms because the puzzle was solved
	for (AAlarmClock* Alarm : RoomAlarms) {
		if (!Alarm) continue;
		Alarm->DeactivateAlarm();
	}
	if (!BoundEntity) {
		UE_LOG(LogClockbait, Warning, TEXT("BoundEntity is invalid in room: %s in MarkPuzzleSolved()"), *RoomId.ToString());
		return;
	}
	AClockbaitGameState* GS = GetWorld()->GetGameState<AClockbaitGameState>();
	if (!GS) return;
	GS->UpdateRoomAIState(RoomId, BoundEntity->GetActorLocation(), EEntityState::Frozen);
	BoundEntity->Freeze(EntityFreezeTime);
}

void AClockbaitRoomVolume::Seal()
{
	bSealed = true;
	SetExitLocked(true);
}

void AClockbaitRoomVolume::GetRoomBounds(FVector& OutOrigin, FVector& OutExtent) const
{
	GetActorBounds(true, OutOrigin, OutExtent);
}

bool AClockbaitRoomVolume::IsPawnInRoom(APawn* Pawn) const
{
	const UShapeComponent* Collision = GetCollisionComponent();
	return Pawn && Collision && Collision->IsOverlappingActor(Pawn);
}

bool AClockbaitRoomVolume::HasPlayerInside() const
{
	bool bPlayersInside = false;
	TArray<AClockbaitCharacter*> Players;
	GetPlayersInside(Players);
	for (auto& Player : Players) {
		if(!Player) continue;
		//Checking is redundant now because AI has its own class
		if (Player->IsPlayerControlled()) {
			bPlayersInside = true;
		}
	}
	return bPlayersInside;
}

void AClockbaitRoomVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(OtherActor)) {
		Player->SetTabletSignal(!bIsRoomDark);
		Player->SetCurrentRoom(this);
		if (HasAuthority() && !bPuzzleSolved) {
			if (EntranceDoor && ExitDoor) {
				EntranceDoor->SetDoorLocked(true);
				ExitDoor->SetDoorLocked(true);
			}
		}
		if (bIsRoomDark) {
			Player->ToggleFlashlight(true);
		}
	}
}

void AClockbaitRoomVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(OtherActor)) {
		if (HasAuthority() && !bSealed) {
			if (EntranceDoor && ExitDoor) {
				if (!bPuzzleSolved) {
					EntranceDoor->SetDoorLocked(false);
				}
				ExitDoor->SetDoorLocked(false);
			}
			if (bPuzzleSolved) {
				if (PuzzleController && Player->IsPlayerControlled()) {
					PuzzleController->NotifyRoomVacated(this);
				}
			}
		}
		Player->SetTabletSignal(false);
		Player->ToggleFlashlight(false);
		Player->SetCurrentRoom(nullptr);
	}
}

void AClockbaitRoomVolume::OnRep_IsRoomDark()
{
	//Enable the lights and signal in dark player's room.
	UE_LOG(LogClockbait, Display, TEXT("On_RepIsRoomDark fired"))
	TArray<AClockbaitCharacter*> Players;
	GetPlayersInside(Players);
	for (auto& Player : Players) {
		Player->ToggleFlashlight(false);
		Player->SetTabletSignal(true);
	}
	for (AActor* Light : RoomLights)
	{
		if (Light) { Light->SetActorHiddenInGame(bIsRoomDark); }
	}
	/*OnRoomDarkChanged(bIsRoomDark);*/
}

void AClockbaitRoomVolume::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AClockbaitRoomVolume, bIsRoomDark);
}

void AClockbaitRoomVolume::GetPlayersInside(TArray<AClockbaitCharacter*>& OutPlayers) const
{
	const UShapeComponent* Collision = GetCollisionComponent();
	TSet<AActor*> ActorsInside;
	if (!Collision) return;
	Collision->GetOverlappingActors(ActorsInside, APawn::StaticClass());
	for (auto& ActorInside : ActorsInside) {
		if (AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(ActorInside)) {
			OutPlayers.Push(Player);
		}
	}
}
