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
#include "Components/WidgetSwitcher.h"
#include "HidingSpot.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "AlarmClock.h"
#include "PuzzleStation.h"
#include "Components/CanvasPanelSlot.h"



void UTabletWidget::SetSignal(bool bNewSignal)
{
	bHasSignal = bNewSignal;
	if(ScreenSwitcher) { ScreenSwitcher->SetActiveWidgetIndex(bHasSignal ? 1 : 0); }
	bSkipInterp = true;
}

void UTabletWidget::ResetEntityPosition()
{
	bSkipInterp = true;
}

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
	OutActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APuzzleStation::StaticClass(), OutActors);

	for (AActor* Actor : OutActors) {
		if (APuzzleStation* Station = Cast<APuzzleStation>(Actor)) {
			Stations.Add(Station);
		}
	}
}

void UTabletWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	OwnMarker->SetVisibility(ESlateVisibility::Collapsed);
	PartnerMarker->SetVisibility(ESlateVisibility::Collapsed);
	PartnerEntityMarker->SetVisibility(ESlateVisibility::Collapsed);

	const ESlateVisibility StaticVis = bHasSignal ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
	for (UImage* Marker : StaticMarkers)
	{
		if (Marker) { Marker->SetVisibility(StaticVis); }
	}

	if (!bHasSignal) return;
	MapSize = MyGeometry.GetLocalSize();
	

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
		if (!Room) continue;
		if (Room->IsPawnInRoom(LocalPawn)) {
			if (Room != BuiltForRoom) {
				BuildStaticMarkers(Room);
			}

			FVector Loc = LocalPawn->GetActorLocation();
			OwnPos = CalculatePositionOnMap(Loc, Room);

			SetMarkerPosition(OwnMarker, OwnPos);
			//reveal the marker once position is set
			OwnMarker->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		if (Room->IsPawnInRoom(Partner)) {
			FVector Loc = Partner->GetActorLocation();
			if (AClockbaitGameState* GameState = GetWorld()->GetGameState<AClockbaitGameState>()) {
				for (const FClockbaitRoomAIState& State : GameState->RoomAIStates) {
					if (State.RoomId == Room->RoomId) {
						FVector EntityLoc = State.AILocation;
						EEntityState StateToSet = State.EnemyState;
						switch (StateToSet) {
						case EEntityState::Alarm:
							PartnerEntityMarker->SetColorAndOpacity(EntityAlarmColor);
							break;
						case EEntityState::Chasing:
							PartnerEntityMarker->SetColorAndOpacity(EntityChaseColor);
							break;
						case EEntityState::Investigating:
							PartnerEntityMarker->SetColorAndOpacity(EntityInvestigateColor);
							break;
						case EEntityState::Frozen:
							PartnerEntityMarker->SetColorAndOpacity(EntityAlarmColor);
							break;
						default:
							PartnerEntityMarker->SetColorAndOpacity(EntityPatrolColor);
							break;
						}
						
						PartnerEntityPos = CalculatePositionOnMap(EntityLoc, Room);
						if (UCanvasPanelSlot* MarkerSlot = Cast<UCanvasPanelSlot>(PartnerEntityMarker->Slot))
						{
							FVector2D PixelPosition = PartnerEntityPos * MapSize;
							if (!bSkipInterp) {
								PixelPosition = FMath::Vector2DInterpTo(CurrentEntityPosition, PixelPosition, InDeltaTime, 6.0f); // 6.0f is the interp speed
							}
							bSkipInterp = false;
							MarkerSlot->SetPosition(PixelPosition);
							CurrentEntityPosition = PixelPosition;

							PartnerEntityMarker->SetVisibility(ESlateVisibility::HitTestInvisible);
						}
					}
				}
			}
			PartnerPos = CalculatePositionOnMap(Loc, Room);
			SetMarkerPosition(PartnerMarker, PartnerPos);

			PartnerMarker->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			PartnerMarker->SetRenderTransformAngle(MapAngleFromActor(Partner));
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

float UTabletWidget::MapAngleFromActor(const AActor* Actor) const
{
	const FVector Fwd = Actor->GetActorForwardVector();
	const FVector2D ScreenDir(-Fwd.Y, Fwd.X);
	return FMath::RadiansToDegrees(FMath::Atan2(ScreenDir.X, -ScreenDir.Y));
}

void UTabletWidget::AddStaticMarker(UCanvasPanel* Canvas, UTexture2D* Texture, const AActor* Actor, AClockbaitRoomVolume* Room, FVector2D Size, float Angle)
{
	if (!Actor) { return; }

	UImage* NewMarker = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	NewMarker->SetBrushFromTexture(Texture);

	UCanvasPanelSlot* MarkerSlot = Canvas->AddChildToCanvas(NewMarker);
	MarkerSlot->SetAutoSize(false);
	MarkerSlot->SetSize(Size);
	MarkerSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	MarkerSlot->SetZOrder(0);

	//Rotates about the marker's centre because the slot is centre-aligned
	NewMarker->SetRenderTransformAngle(Angle);

	StaticMarkers.Add(NewMarker);
	SetMarkerPosition(NewMarker, CalculatePositionOnMap(Actor->GetActorLocation(), Room));
}

void UTabletWidget::BuildStaticMarkers(AClockbaitRoomVolume* Room)
{
	for (UImage* Marker : StaticMarkers)
	{
		if (Marker) { Marker->RemoveFromParent(); }
	}
	StaticMarkers.Empty();

	UCanvasPanel* Canvas = Cast<UCanvasPanel>(OwnMarker->GetParent());
	if (!Canvas || !Room) { return; }

	for (AAlarmClock* Alarm : Room->RoomAlarms)
	{
		if (!Alarm) continue;
		AddStaticMarker(Canvas, AlarmMarkerTexture, Alarm, Room, StaticMarkerSize, 0.0f);
	}
	for (AHidingSpot* Hideout : Room->RoomHidingSpots) {
		if (!Hideout) continue;
		AddStaticMarker(Canvas, HidingSpotMarkerTexture, Hideout, Room, StaticMarkerSize, 0.0f);
	}

	FVector Origin, Extent;
	Room->GetRoomBounds(Origin, Extent);
	const FBox RoomBox(Origin - Extent, Origin + Extent);
	for (APuzzleStation* Station : Stations) {
		if (!Station || !RoomBox.IsInside(Station->GetActorLocation())) continue;
		AddStaticMarker(Canvas, Station->MapIcon, Station, Room, Station->MapIconSize,
			MapAngleFromActor(Station) + Station->MapIconAngleOffset);
	}
	BuiltForRoom = Room;
}
