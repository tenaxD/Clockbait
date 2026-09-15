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

	UPROPERTY(BlueprintReadOnly, Category = "Tablet")
	bool bEntitySeesPartner{ false };

protected:

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FVector2D CalculatePositionOnMap(FVector Position, class AClockbaitRoomVolume* RoomToCheck);

	void SetMarkerPosition(class UImage* Marker, FVector2D Frac);

	UPROPERTY(VisibleAnywhere, Category = "Tablet")
	TArray<class AClockbaitRoomVolume*> Rooms;

	FVector2D OwnPos = FVector2D::ZeroVector;

	FVector2D PartnerPos = FVector2D::ZeroVector;

	FVector2D PartnerEntityPos = FVector2D::ZeroVector;

	FVector2D CurrentEntityPosition = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere)
	FVector2D MapSize{ 600.0f, 800.0f };

	UPROPERTY(meta = (BindWidget))
	class UImage* OwnMarker;

	UPROPERTY(meta = (BindWidget))
	class UImage* PartnerMarker;

	UPROPERTY(meta = (BindWidget))
	class UImage* PartnerEntityMarker;

	UPROPERTY(EditAnywhere)
	FLinearColor EntityIdleColor;

	UPROPERTY(EditAnywhere)
	FLinearColor EntityAlertColor;
};
