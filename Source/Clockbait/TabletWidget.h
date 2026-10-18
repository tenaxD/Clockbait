// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TabletWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKBAIT_API UTabletWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetSignal(bool bNewSignal);

	void ResetEntityPosition();

protected:

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FVector2D CalculatePositionOnMap(FVector Position, class AClockbaitRoomVolume* RoomToCheck);

	void SetMarkerPosition(class UImage* Marker, FVector2D Frac);

	void BuildStaticMarkers(class AClockbaitRoomVolume* Room);

	void AddStaticMarker(class UCanvasPanel* Canvas, class UTexture2D* Texture,
		const AActor* Actor, class AClockbaitRoomVolume* Room, FVector2D Size, float Angle);

	//World yaw expressed in the map's rotated/flipped axes, same swap CalculatePositionOnMap does
	float MapAngleFromActor(const AActor* Actor) const;

	UPROPERTY(VisibleAnywhere, Category = "Tablet")
	TArray<class AClockbaitRoomVolume*> Rooms;

	UPROPERTY(VisibleAnywhere, Category = "Tablet")
	TArray<class APuzzleStation*> Stations;

	FVector2D OwnPos = FVector2D::ZeroVector;

	FVector2D PartnerPos = FVector2D::ZeroVector;

	FVector2D PartnerEntityPos = FVector2D::ZeroVector;

	FVector2D CurrentEntityPosition = FVector2D::ZeroVector;

	bool bSkipInterp{ true };

	FVector2D MapSize{ 600.0f, 800.0f }; //set in tick so this value might be inacurrate

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* ScreenSwitcher;

	bool bHasSignal{ false };

	UPROPERTY(meta = (BindWidget))
	class UImage* OwnMarker;

	UPROPERTY(meta = (BindWidget))
	class UImage* PartnerMarker;

	UPROPERTY(meta = (BindWidget))
	class UImage* PartnerEntityMarker;

	UPROPERTY()
	class AClockbaitRoomVolume* BuiltForRoom{ nullptr };

	UPROPERTY()
	TArray<class UImage*> StaticMarkers;

	UPROPERTY(EditDefaultsOnly, Category = "Tablet")
	class UTexture2D* AlarmMarkerTexture = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Tablet")
	class UTexture2D* HidingSpotMarkerTexture = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Tablet")
	FVector2D StaticMarkerSize{ 32.0f, 32.0f };

	UPROPERTY(EditAnywhere)
	FLinearColor EntityAlarmColor;
	
	UPROPERTY(EditAnywhere)
	FLinearColor EntityChaseColor;

	UPROPERTY(EditAnywhere)
	FLinearColor EntityInvestigateColor;

	UPROPERTY(EditAnywhere)
	FLinearColor EntityPatrolColor;
};
