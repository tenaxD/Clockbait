// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleDecal.h"
#include "Components/SceneComponent.h"
#include "Components/DecalComponent.h"
#include "Components/BillboardComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Clockbait.h"

// Sets default values
APuzzleDecal::APuzzleDecal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(RootComponent);

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(Decal);
	Billboard->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void APuzzleDecal::BeginPlay()
{
	Super::BeginPlay();

	// Nothing here is replicated or gated by authority: Symbol and Color are saved into the level, so
	// every machine loads the same pair and paints the same wall without being told.
	DecalMaterial = Decal->CreateDynamicMaterialInstance();
	if (!DecalMaterial)
	{
		UE_LOG(LogClockbait, Warning, TEXT("PuzzleDecal '%s': the decal has no material, so nothing will be painted"), *GetName());
		return;
	}

	if (SymbolTextures.Find(Symbol))
	{
		DecalMaterial->SetTextureParameterValue(TEXT("Symbol"), SymbolTextures[Symbol]);
	}
	else
	{
		UE_LOG(LogClockbait, Warning, TEXT("PuzzleDecal '%s': nothing in SymbolTextures for symbol %s"),
			*GetName(), *UEnum::GetValueAsString(Symbol));
	}

	if (Colors.Find(Color))
	{
		DecalMaterial->SetVectorParameterValue(TEXT("Color"), Colors[Color]);
	}
	else
	{
		UE_LOG(LogClockbait, Warning, TEXT("PuzzleDecal '%s': nothing in Colors for colour %s"),
			*GetName(), *UEnum::GetValueAsString(Color));
	}
}

// Called every frame
void APuzzleDecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
