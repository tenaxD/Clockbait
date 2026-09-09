// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PuzzleTypes.generated.h"

UENUM(BlueprintType)
enum class EPuzzleColor : uint8
{
	Red,
	Green,
	Blue,
	Yellow,
	Purple,
	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EPuzzleSymbol : uint8
{
	Flower,
	Dog,
	Sun,
	Water,
	Fire,
	Math
};
