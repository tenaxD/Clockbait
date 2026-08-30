// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutomaticDoor.generated.h"

UCLASS()
class CLOCKBAIT_API AAutomaticDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAutomaticDoor();

	UFUNCTION()
	void SetDoorLocked(bool bNewLocked);

	UFUNCTION(BlueprintPure)
	bool IsDoorLocked() const { return bLocked; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Locked)
	bool bLocked = false;

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_Locked();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
