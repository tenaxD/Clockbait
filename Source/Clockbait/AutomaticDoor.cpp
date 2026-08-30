// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomaticDoor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AAutomaticDoor::AAutomaticDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

void AAutomaticDoor::SetDoorLocked(bool bNewLocked)
{
	if (bLocked == bNewLocked) return;

	if (HasAuthority()) {
		bLocked = bNewLocked;
		OnRep_Locked();
	}
}

void AAutomaticDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAutomaticDoor, bLocked);
}

// Called when the game starts or when spawned
void AAutomaticDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAutomaticDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

