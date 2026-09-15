// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorPuzzleButton.h"
#include "Clockbait.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
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

	ButtonMaterial = ButtonMesh->CreateDynamicMaterialInstance(0);

}

void AColorPuzzleButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AColorPuzzleButton, CurrentColor);
}

void AColorPuzzleButton::ApplyColor()
{
	if (!ButtonMaterial || !Colors.Find(CurrentColor))
	{
		return;
	}

	ButtonMaterial->SetVectorParameterValue(TEXT("ButtonColor"), Colors[CurrentColor]);
}

void AColorPuzzleButton::OnRep_ColorChanged()
{
	ApplyColor();
	OnColorChanged();
}

void AColorPuzzleButton::ChangeColor()
{
	if (Colors.Num() != static_cast<uint8>(EPuzzleColor::Count)) return;
	UE_LOG(LogClockbait, Display, TEXT("Color changed"));
	CurrentColor = static_cast<EPuzzleColor>(
		((static_cast<uint8>(CurrentColor) + 1) % static_cast<uint8>(EPuzzleColor::Count))
		);
	OnRep_ColorChanged();
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

	ButtonMaterial = ButtonMesh->CreateDynamicMaterialInstance(0);
	if (!ButtonMaterial)
	{
		UE_LOG(LogClockbait, Warning, TEXT("ColorPuzzleButton '%s': material slot 0 is empty, so the button cannot change colour"), *GetName());
		return;
	}

	if (SymbolTextures.Find(Symbol))
	{
		ButtonMaterial->SetTextureParameterValue(TEXT("SymbolMask"), SymbolTextures[Symbol]);
	}
	else
	{
		UE_LOG(LogClockbait, Warning, TEXT("ColorPuzzleButton '%s': nothing in SymbolTextures for symbol %s"),
			*GetName(), *UEnum::GetValueAsString(Symbol));
	}

	ApplyColor();
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

