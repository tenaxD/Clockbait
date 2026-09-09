// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PuzzleStation.h"
#include "ColorPuzzleButton.h"
#include "PuzzleTypes.h"
#include "ColorPuzzleStation.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKBAIT_API AColorPuzzleStation : public APuzzleStation
{
	GENERATED_BODY()
	
public:
	virtual bool CheckSolution() const override;

	void SetSolution(const TMap<EPuzzleSymbol, EPuzzleColor>& Sol);
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<AColorPuzzleButton*> Buttons;

	TMap<EPuzzleSymbol, EPuzzleColor> Solution;


};
