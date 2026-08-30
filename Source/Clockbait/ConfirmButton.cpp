// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfirmButton.h"
#include "PuzzleStation.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ClockbaitCharacter.h"
#include "Clockbait.h"

// Sets default values
AConfirmButton::AConfirmButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	RootComponent = SceneRoot;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button Mesh"));
	ButtonMesh->SetupAttachment(RootComponent);
}

void AConfirmButton::Interact_Implementation(APawn* Interactor)
{
	if (PuzzleStation) {
		PuzzleStation->Confirm();
	}
}

// Called when the game starts or when spawned
void AConfirmButton::BeginPlay()
{
	Super::BeginPlay();
	
}

void AConfirmButton::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(PC->GetCharacter());
	if (!Player) return;

	Player->ServerInteract(this);
	UE_LOG(LogClockbait, Display, TEXT("Puzzle Confirmed"));
}

// Called every frame
void AConfirmButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

