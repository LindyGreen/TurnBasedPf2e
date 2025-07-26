

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
	IsInRange    UMETA(DisplayName = "Is In Range"),
	IsSpellAOE   UMETA(DisplayName = "Is Spell AOE")
};
