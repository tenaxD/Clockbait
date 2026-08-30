// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "PuzzleStation.generated.h"

UCLASS(Abstract)
class CLOCKBAIT_API APuzzleStation : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APuzzleStation();
	
	bool IsSolved() const { return bSolved; }

	void Confirm();

	virtual bool CheckSolution() const PURE_VIRTUAL(APuzzleStation::CheckSolution, return false;)

	virtual void Interact_Implementation(APawn* Interactor) override;

	/** Buttons stop responding once the station is solved, so the arrangement can't be undone while the pair escapes. */
	bool AreButtonsLocked() const { return bButtonsLocked; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void EnterPuzzle(APawn* PlayerRef);

	void ExitPuzzle(APawn* PlayerRef);
	
	/*UPROPERTY(EditAnywhere)
	class APuzzleController* Controller{ nullptr };*/

	UPROPERTY(VisibleAnywhere, Category = "Puzzle")
	class UCameraComponent* PuzzleCamera{ nullptr };

	/** The room this station belongs to. Asked to open its exit on success - the door pointer itself lives on the room. */
	UPROPERTY(EditAnywhere, Category = "Puzzle")
	class AClockbaitRoomVolume* Room{ nullptr };

	UPROPERTY()
	APawn* OccupyingPlayer{ nullptr };

	bool bSolved{ false };

	bool bButtonsLocked{ false };

	UPROPERTY(EditAnywhere, Category = "Puzzle")
	float InteractionLockTime = 0.5f;

	bool bInteractionLocked{ false };

	FTimerHandle InteractionLockTimer;

	void EnableInteraction();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
