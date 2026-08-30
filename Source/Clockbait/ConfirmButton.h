// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "ConfirmButton.generated.h"

UCLASS()
class CLOCKBAIT_API AConfirmButton : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConfirmButton();

	virtual void Interact_Implementation(APawn* Interactor) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class APuzzleStation* PuzzleStation{ nullptr };

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ButtonMesh{};
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
