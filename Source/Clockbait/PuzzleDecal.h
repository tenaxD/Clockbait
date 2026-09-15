// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleTypes.h"
#include "PuzzleDecal.generated.h"

/**
 * One symbol->colour fact painted on a wall. The pair is stored as puzzle data rather than as
 * finished artwork, so a clue can never show a colour no button is able to produce, and so the
 * solution can eventually be randomised per run with the clues following it.
 */
UCLASS()
class CLOCKBAIT_API APuzzleDecal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APuzzleDecal();

	UPROPERTY(EditAnywhere, Category = "Clue")
	EPuzzleSymbol Symbol{};

	UPROPERTY(EditAnywhere, Category = "Clue")
	EPuzzleColor Color{};
	
	UPROPERTY(EditAnywhere, Category = "Clue")
	TMap<EPuzzleSymbol, UTexture2D*> SymbolTextures;

	UPROPERTY(EditAnywhere, Category = "Clue")
	TMap<EPuzzleColor, FLinearColor> Colors;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Clue")
	class UDecalComponent* Decal{ nullptr };

	UPROPERTY(VisibleAnywhere, Category = "Clue")
	class UBillboardComponent* Billboard{ nullptr };

	/** This clue's private copy of the decal material - see AColorPuzzleButton::ButtonMaterial. */
	UPROPERTY()
	class UMaterialInstanceDynamic* DecalMaterial{ nullptr };

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
