// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "PuzzleTypes.h"
#include "ColorPuzzleButton.generated.h"

UCLASS()
class CLOCKBAIT_API AColorPuzzleButton : public AActor, public IInteractable
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AColorPuzzleButton();

	UPROPERTY(ReplicatedUsing=OnRep_ColorChanged)
	EPuzzleColor CurrentColor = EPuzzleColor::Red;

	UPROPERTY(EditAnywhere)
	EPuzzleSymbol Symbol{};

	UPROPERTY(EditAnywhere)
	TMap<EPuzzleSymbol, UTexture2D*> SymbolTextures;

	UPROPERTY(EditAnywhere)
	TMap<EPuzzleColor, FLinearColor> Colors;

	/**
	 * This button's private copy of the material. Parameters cannot be set on the material asset
	 * itself - every button shares that one - so each button wraps it in a dynamic instance.
	 */
	UPROPERTY()
	class UMaterialInstanceDynamic* ButtonMaterial{ nullptr };

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_ColorChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void OnColorChanged();

	
	void ApplyColor();

	void ChangeColor();

	virtual void Interact_Implementation(APawn* Interactor) override;

	void SetStation(class APuzzleStation* InStation) { Station = InStation; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ButtonMesh{ nullptr };

	/** The station this button belongs to. Consulted so a solved station's buttons stop responding. */
	class APuzzleStation* Station{ nullptr };
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
