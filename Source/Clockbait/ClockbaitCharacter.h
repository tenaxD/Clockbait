// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ClockbaitCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USpotLightComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
/**
 *  A basic first person character
 */
UCLASS(abstract)
class AClockbaitCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* Flashlight;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* InteractAction;
	
public:
	AClockbaitCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Requests the server to activate an alarm target hit by the local player's line trace */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Alarm")
	void ServerActivateAlarm(AActor* AlarmTarget);

	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* Target);

	/** Server-only: called by AHidingSpot when this player enters/exits a hiding spot. Not an RPC itself, hiding spots already validate over their own RPCs. */
	UFUNCTION(BlueprintCallable, Category = "Hiding")
	void SetHiddenFromAI(bool bNewHiddenFromAI);

	bool IsHiddenFromAI() const { return bIsHiddenFromAI; }

	void ToggleFlashlight(bool bTurnOn);

protected:

	/** Implemented in Blueprint: runs on the server, contains the cooldown-check + Activate logic */
	UFUNCTION(BlueprintImplementableEvent, Category = "Alarm")
	void OnServerActivateAlarm(AActor* AlarmTarget);

	/** Implemented in Blueprint: runs on every client (including the owner), for hiding cosmetics (crouch pose, mesh visibility, etc.) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Hiding")
	void OnHiddenFromAIChanged(bool bNewHiddenFromAI);

	UFUNCTION()
	void OnRep_IsHiddenFromAI();

	// Whether this player is currently tucked away in a hiding spot and should be excluded from AI perception.
	// Replicated (not just server-side) so OnHiddenFromAIChanged can drive cosmetics on every client, not just the server.
	UPROPERTY(ReplicatedUsing = OnRep_IsHiddenFromAI)
	bool bIsHiddenFromAI = false;

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UPROPERTY(EditAnywhere)
	float maxInteractionDist{ 300.0f };

	UPROPERTY(EditAnywhere)
	float InteractionSphereRadius{ 30.0f };


protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }


private:
	FTimerHandle TraceTimerHandle;

	void PerformInteractionTrace();

	AActor* FocusedActor = nullptr;

	void Interact();
};

