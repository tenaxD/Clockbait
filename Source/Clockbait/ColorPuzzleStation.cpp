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
		const bool bSlotMatches = Buttons[i]->CurrentColor == Solution[i];

		// Names the actual instance so a Buttons array pointing at the wrong actors is visible,
		// and prints every slot rather than stopping at the first mismatch.
		UE_LOG(LogClockbait, Display, TEXT("CheckSolution slot %d: %s is %s, expected %s -> %s"),
			i,
			*GetNameSafe(Buttons[i]),
			*UEnum::GetValueAsString(Buttons[i]->CurrentColor),
			*UEnum::GetValueAsString(Solution[i]),
			bSlotMatches ? TEXT("OK") : TEXT("MISMATCH"));

		if (!bSlotMatches) {
			bMatches = false;
		}
	}
	return bMatches;
}