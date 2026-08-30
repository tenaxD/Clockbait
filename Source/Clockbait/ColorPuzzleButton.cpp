// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorPuzzleButton.h"
#include "Clockbait.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "ClockbaitCharacter.h"
#include "PuzzleStation.h"

// Sets default values
AColorPuzzleButton::AColorPuzzleButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(RootComponent);

}

void AColorPuzzleButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AColorPuzzleButton, CurrentColor);
}

void AColorPuzzleButton::OnRep_ColorChanged()
{
	ButtonMesh->SetMaterial(0, Colors[static_cast<uint8>(CurrentColor)]);
}

void AColorPuzzleButton::ChangeColor()
{
	if (Colors.Num() != static_cast<uint8>(EPuzzleColor::Count)) return;
	UE_LOG(LogClockbait, Display, TEXT("Color changed"));
	CurrentColor = static_cast<EPuzzleColor>(
		((static_cast<uint8>(CurrentColor) + 1) % static_cast<uint8>(EPuzzleColor::Count))
		);
	OnRep_ColorChanged();
	OnColorChanged();
}

void AColorPuzzleButton::Interact_Implementation(APawn* Interactor)
{
	if (Station && Station->AreButtonsLocked())
	{
		return;
	}

	ChangeColor();
}

// Called when the game starts or when spawned
void AColorPuzzleButton::BeginPlay()
{
	Super::BeginPlay();
	
}

void AColorPuzzleButton::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	AClockbaitCharacter* Player = Cast<AClockbaitCharacter>(PC->GetCharacter());
	if (!Player) return;

	Player->ServerInteract(this);
}

// Called every frame
void AColorPuzzleButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

