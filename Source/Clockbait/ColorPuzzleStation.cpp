// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorPuzzleStation.h"
#include "Clockbait.h"

bool AColorPuzzleStation::CheckSolution() const
{
	if (Buttons.Num() < 1 || Buttons.Num() != Solution.Num()) {
		UE_LOG(LogClockbait, Warning, TEXT("Button array or Solution not filled properly"));
		return false;
	}
	bool bMatches = true;
	for (auto i = 0; i < Buttons.Num(); ++i) {
		// Every configuration problem below fails the check rather than skipping the slot: a station
		// that is wired wrong must never report itself solved and open the doors for free.
		if (!Buttons[i]) {
			bMatches = false;
			UE_LOG(LogClockbait, Warning, TEXT("Button at index %d does not exist"), i);
			continue;
		}

		const EPuzzleColor* Expected = Solution.Find(Buttons[i]->Symbol);
		if (!Expected) {
			bMatches = false;
			UE_LOG(LogClockbait, Warning, TEXT("The symbol of button %s which is %s does not exist in the solution"),
				*GetNameSafe(Buttons[i]),
				*UEnum::GetValueAsString(Buttons[i]->Symbol));
			continue;
		}

		const bool bSlotMatches = Buttons[i]->CurrentColor == *Expected;

		// Names the actual instance so a Buttons array pointing at the wrong actors is visible,
		// and prints every slot rather than stopping at the first mismatch.
		UE_LOG(LogClockbait, Display, TEXT("CheckSolution slot %d: %s is %s, expected %s -> %s"),
			i,
			*GetNameSafe(Buttons[i]),
			*UEnum::GetValueAsString(Buttons[i]->CurrentColor),
			*UEnum::GetValueAsString(*Expected),
			bSlotMatches ? TEXT("OK") : TEXT("MISMATCH"));

		if (!bSlotMatches) {
			bMatches = false;
		}
	}
	return bMatches;
}

void AColorPuzzleStation::SetSolution(const TMap<EPuzzleSymbol, EPuzzleColor>& Sol)
{
	Solution = Sol;
}

void AColorPuzzleStation::BeginPlay()
{
	Super::BeginPlay();

	for (auto i = 0; i < Buttons.Num(); ++i) {
		if (!Buttons[i]) {
			UE_LOG(LogClockbait, Warning, TEXT("ColorPuzzleStation '%s': Buttons slot %d is empty"), *GetName(), i);
			continue;
		}

		Buttons[i]->SetStation(this);
	}
}
