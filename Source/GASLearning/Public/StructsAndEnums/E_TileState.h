

#pragma once

#include "CoreMinimal.h"
#include "E_TileState.generated.h"

UENUM(BlueprintType)
enum class ETileState : uint8
{
	None         UMETA(DisplayName = "None"),
	Hovered      UMETA(DisplayName = "Hovered"),
	Selected     UMETA(DisplayName = "Selected"),
	IsReachable  UMETA(DisplayName = "Is Reachable"),
	IsReachable2 UMETA(DisplayName = "Is 2A Reachable"),
	IsInRange    UMETA(DisplayName = "Is In Range"),
	IsInRange2   UMETA(DisplayName = "Is In 2n Range"),
	IsSpellAOE   UMETA(DisplayName = "Is Spell AOE")
};
