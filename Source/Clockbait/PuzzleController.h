// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleTypes.h"
#include "PuzzleController.generated.h"

UCLASS()
class CLOCKBAIT_API APuzzleController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APuzzleController();

	void NotifyStationSolved(class APuzzleStation* Station);

	void NotifyStationFailed(class APuzzleStation* Station, APawn* PlayerPawn);

	void NotifyRoomVacated(class AClockbaitRoomVolume* Room);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	TArray<class APuzzleStation*> Stations;

	bool bSegmentComplete{ false };

	UPROPERTY(EditAnywhere)
	TMap<EPuzzleSymbol, EPuzzleColor> FinalSolution;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
