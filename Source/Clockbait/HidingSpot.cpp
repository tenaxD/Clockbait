// Copyright Epic Games, Inc. All Rights Reserved.

#include "HidingSpot.h"
#include "ClockbaitCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Clockbait.h"
#include "Components/CapsuleComponent.h"

AHidingSpot::AHidingSpot()
{
	bReplicates = true;
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = SceneRoot;
	HidingPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Hiding Camera Position"));
	HidingPosition->SetupAttachment(RootComponent);
}

void AHidingSpot::BeginPlay()
{
	Super::BeginPlay();
}

void AHidingSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHidingSpot, OccupyingPawn);
}

void AHidingSpot::Interact_Implementation(APawn* Interactor)
{
	if (OccupyingPawn == Interactor) {
		ExitHiding(Interactor);
	}
	else {
		EnterHiding(Interactor);
	}
}

void AHidingSpot::EnterHiding(APawn* RequestingPawn)
{
	UE_LOG(LogClockbait, Warning, TEXT("EnterHiding: Authority=%d Role=%d"),
		HasAuthority(), (int32)GetLocalRole());
	// Reject rather than queue/replace - occupancy conflicts shouldn't happen in practice since the
	// two players are always in different rooms, so a hard no-op here is enough.
	if (!RequestingPawn || OccupyingPawn != nullptr)
	{
		return;
	}

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Player %s is now hidden"), *GetNameSafe(RequestingPawn)));
	}
	OccupyingPawn = RequestingPawn;

	// Set directly in C++ (not via the BlueprintImplementableEvent below) because AI perception
	// correctness depends on it - this must always happen, not be left to whatever the BP graph does.
	if (AClockbaitCharacter* Character = Cast<AClockbaitCharacter>(RequestingPawn))
	{
		Character->SetHiddenFromAI(true);
		Character->SetActorLocation(HidingPosition->GetComponentLocation());
		Character->GetCharacterMovement()->DisableMovement();
		Character->ClientLockLookRotation();
		Character->CurrentHidingSpot = this;
	}

	OnRep_OccupyingPawn(); // OnRep only fires on clients; call manually so the server also gets OnHidingOccupancyChanged
	OnServerEnterHiding(RequestingPawn);
}

void AHidingSpot::ExitHiding(APawn* RequestingPawn)
{
	if (RequestingPawn != OccupyingPawn)
	{
		return;
	}

	AClockbaitCharacter* Character = Cast<AClockbaitCharacter>(RequestingPawn);
	if (!Character)
	{
		return;
	}

	const float HalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float Radius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

	FVector ExitDir = Character->GetFirstPersonCameraComponent()->GetForwardVector().GetSafeNormal2D();
	if (ExitDir.IsNearlyZero())
	{
		ExitDir = GetActorForwardVector().GetSafeNormal2D();
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Character);

	//Walk outwards from the spot itself, not from the player - while hidden they sit under the floor
	const FVector ProbeStart = GetActorLocation() + FVector(0.0f, 0.0f, HalfHeight);
	FVector ExitXY = ProbeStart + ExitDir * HidingExitDist;

	FHitResult WallHit;
	if (GetWorld()->LineTraceSingleByChannel(WallHit, ProbeStart, ExitXY, ECC_Visibility, Params))
	{
		ExitXY = WallHit.ImpactPoint - ExitDir * Radius;
	}

	FHitResult FloorHit;
	if (!GetWorld()->LineTraceSingleByChannel(FloorHit,
		ExitXY + FVector(0.0f, 0.0f, HalfHeight),
		ExitXY - FVector(0.0f, 0.0f, 500.0f), ECC_Visibility, Params))
	{
		UE_LOG(LogClockbait, Warning, TEXT("Can't leave '%s': no floor under the exit point"), *GetNameSafe(this));
		return;
	}

	// Only give up occupancy once the exit is guaranteed to succeed. Releasing it earlier and then
	// bailing out would leave the player frozen and hidden while the spot considered itself free.
	OccupyingPawn = nullptr;

	Character->SetHiddenFromAI(false);
	Character->SetActorLocation(FloorHit.ImpactPoint + FVector(0.0f, 0.0f, HalfHeight));
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Character->ClientUnlockLookRotation();
	Character->CurrentHidingSpot = nullptr;
	Character->ClearCompromised();
	UE_LOG(LogClockbait, Display, TEXT("Exit from Hiding Spot succeeded"));

	OnRep_OccupyingPawn();
	OnServerExitHiding(RequestingPawn);
}

void AHidingSpot::OnRep_OccupyingPawn()
{
	OnHidingOccupancyChanged(OccupyingPawn);
}
