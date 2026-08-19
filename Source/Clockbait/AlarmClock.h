// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "AlarmClock.generated.h"

UCLASS()
class CLOCKBAIT_API AAlarmClock : public AActor, public IInteractable 
{
	GENERATED_BODY()
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:	
	// Sets default values for this actor's properties
	AAlarmClock();
	bool IsOnCooldown() const { return bOnCooldown;}

	UFUNCTION(BlueprintCallable)
	void TryActivate();

	void ActivateAlarm ();
	
	UFUNCTION(BlueprintCallable)
	void DeactivateAlarm();

	UFUNCTION(BlueprintImplementableEvent)
	void MulticastPlayRingingSound();

	UFUNCTION(BlueprintImplementableEvent)
	void MulticastStopRingingSound();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APawn* AssignedEnemy = nullptr;

protected:

	bool bOnCooldown{ false };
	FTimerHandle CooldownTimerHandle;

	void OnCooldownExpired();

	UPROPERTY(EditAnywhere)
	float CooldownDuration = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Alarm")
	float EnemyProximityThreshold = 150.0f;

	UPROPERTY(VisibleAnywhere, Category = "Alarm")
	USceneComponent* NavTarget;

	virtual void Interact_Implementation(APawn* Interactor) override;
private:	
	

};
