// Fill out your copyright notice in the Description page of Project Settings.


#include "TabletWidget.h"
#include "Clockbait.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "ClockbaitRoomVolume.h"
#include "ClockbaitCharacter.h"
#include "ClockbaitGameState.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"



void UTabletWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AClockbaitRoomVolume::StaticClass(), OutActors);

	for (auto& Actor : OutActors) {
		if (AClockbaitRoomVolume* Room = Cast<AClockbaitRoomVolume>(Actor)) {
			UE_LOG(LogClockbait, Display, TEXT("Added Room to TabletWidget"));
			Rooms.Add(Room);
		}
	}
}

void UTabletWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	APawn* LocalPawn = GetOwningPlayerPawn();
	if (!LocalPawn) return;
	if (!GetWorld()->GetGameState()) return;

	APawn* Partner{ nullptr };
	const TArray<APlayerState*>& Players = GetWorld()->GetGameState()->PlayerArray;
	for (auto& Player : Players) {
		if (Player == LocalPawn->GetPlayerState()) continue;
		Partner = Player->GetPawn();
		break;
	}

	for (auto& Room : Rooms) {
		if (Room->IsPawnInRoom(LocalPawn)) {
			FVector Loc = LocalPawn->GetActorLocation();
			OwnPos = CalculatePositionOnMap(Loc, Room);
			SetMarkerPosition(OwnMarker, OwnPos);
		}
		if (Room->IsPawnInRoom(Partner)) {
			FVector Loc = Partner->GetActorLocation();
			if (AClockbaitGameState* GameState = GetWorld()->GetGameState<AClockbaitGameState>()) {
				for (const FClockbaitRoomAIState& State : GameState->RoomAIStates) {
					if (State.RoomId == Room->RoomId) {
						FVector EntityLoc = State.AILocation;
						bEntitySeesPartner = State.bIsPerceivingPlayer;
						if (bEntitySeesPartner) {
							PartnerEntityMarker->SetColorAndOpacity(EntityAlertColor);
						}
						else {
							PartnerEntityMarker->SetColorAndOpacity(EntityIdleColor);
						}
						PartnerEntityPos = CalculatePositionOnMap(EntityLoc, Room);
						if (UCanvasPanelSlot* MarkerSlot = Cast<UCanvasPanelSlot>(PartnerEntityMarker->Slot))
						{
							FVector2D PixelPosition = PartnerEntityPos * MapSize;
							if (CurrentEntityPosition != FVector2D::ZeroVector) {
								PixelPosition = FMath::Vector2DInterpTo(CurrentEntityPosition, PixelPosition, InDeltaTime, 6.0f); // 6.0f is the interp speed
							}
							MarkerSlot->SetPosition(PixelPosition);
							CurrentEntityPosition = PixelPosition;
						}
					}
				}
			}
			PartnerPos = CalculatePositionOnMap(Loc, Room);
			SetMarkerPosition(PartnerMarker, PartnerPos);
			FVector Fwd = Partner->GetActorForwardVector();
			FVector2D ScreenDir(-Fwd.Y, Fwd.X);
			auto Rads = FMath::Atan2(ScreenDir.X, -ScreenDir.Y);
			auto Angle = FMath::RadiansToDegrees(Rads);
			PartnerMarker->SetRenderTransformAngle(Angle);
		}
	}
}



FVector2D UTabletWidget::CalculatePositionOnMap(FVector Position, AClockbaitRoomVolume* RoomToCheck)
{
	FVector Origin, Extent;
	RoomToCheck->GetRoomBounds(Origin, Extent);
	FVector Frac = (Position - (Origin - Extent)) / (2 * Extent);
	return FVector2D(1.0f - Frac.Y, Frac.X);
}

void UTabletWidget::SetMarkerPosition(UImage* Marker, FVector2D Frac)
{
	if (UCanvasPanelSlot* MarkerSlot = Cast<UCanvasPanelSlot>(Marker->Slot)) {
		FVector2D PixelPosition = Frac * MapSize;
		MarkerSlot->SetPosition(PixelPosition);
	}
}
