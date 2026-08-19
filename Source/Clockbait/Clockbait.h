// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogClockbait, Log, All);

namespace ClockbaitCollision
{
	constexpr ECollisionChannel Interaction = ECC_GameTraceChannel2;
}