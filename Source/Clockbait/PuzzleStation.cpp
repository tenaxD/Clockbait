// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleStation.h"
#include "Camera/CameraComponent.h"
#include "ClockbaitPlayerController.h"
#include "ClockbaitCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClockbaitRoomVolume.h"
#include "PuzzleController.h"
#include "Kismet/GameplayStatics.h"
#include "Clockbait.h"

// Sets default values
APuzzleStation::APuzzleStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	PuzzleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Puzzle Camera"));
	PuzzleCamera->SetupAttachment(RootComponent);
}

void APuzzleStation::Confirm()
{
	// Solving is one-way: a second confirmation on an already-solved station is a no-op, rather than
	// something that could flip bSolved back and re-lock the door the pair is running for.
	if (bSolved)
	{
		return;
	}

	bSolved = CheckSolution();

	if (!bSolved)
	{
		if (PuzzleController)
		{
			PuzzleController->NotifyStationFailed(this, OccupyingPlayer);
			ExitPuzzle(OccupyingPlayer);
		}
		return;
	}
	UE_LOG(LogClockbait, Display, TEXT("Puzzle Solved"));
	bButtonsLocked = true;

	// ExitPuzzle clears OccupyingPlayer, so read it before calling.
	if (APawn* SolvingPlayer = OccupyingPlayer)
	{
		ExitPuzzle(SolvingPlayer);
	}

	if (PuzzleController)
	{
		PuzzleController->NotifyStationSolved(this);
	}
}

// Called when the game starts or when spawned
void APuzzleStation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APuzzleStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APuzzleStation::Interact_Implementation(APawn* Interactor) {
	if (bInteractionLocked) return;

	if (OccupyingPlayer == Interactor)
	{
		ExitPuzzle(Interactor);
	}
	else {
		EnterPuzzle(Interactor);
	}
}

void APuzzleStation::EnterPuzzle(APawn* PlayerRef)
{
	if (OccupyingPlayer || !PlayerRef) return;

	bInteractionLocked = true;
	GetWorldTimerManager().SetTimer(InteractionLockTimer, this, &APuzzleStation::EnableInteraction, InteractionLockTime, false);

	if (AClockbaitCharacter* Character = Cast<AClockbaitCharacter>(PlayerRef)) {
		AClockbaitPlayerController* PC = Character->GetController<AClockbaitPlayerController>();
		if (!PC) return;
		OccupyingPlayer = Character;
		PC->EnterPuzzleMode(this, InteractionLockTime);
		Character->GetCharacterMovement()->DisableMovement();
	}
}
void APuzzleStation::PlayBuzzerSound_Implementation() {
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), BuzzerSound, GetActorLocation());
}

void APuzzleStation::ExitPuzzle(APawn* PlayerRef)
{
	if (PlayerRef != OccupyingPlayer) return;

	bInteractionLocked = true;
	GetWorldTimerManager().SetTimer(InteractionLockTimer, this, &APuzzleStation::EnableInteraction, InteractionLockTime, false);

	if (AClockbaitCharacter* Character = Cast < AClockbaitCharacter>(PlayerRef)) {
		AClockbaitPlayerController* PC = Character->GetController<AClockbaitPlayerController>();
		if (!PC) return;
		OccupyingPlayer = nullptr;
		PC->ExitPuzzleMode(InteractionLockTime);
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void APuzzleStation::EnableInteraction()
{
	bInteractionLocked = false;
}
