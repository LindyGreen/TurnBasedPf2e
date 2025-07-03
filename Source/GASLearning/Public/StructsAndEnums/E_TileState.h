

#pragma once

#include "CoreMinimal.h"
#include "E_TileState.generated.h"

UENUM(BlueprintType)
enum class ETileState : uint8
{
	None         UMETA(DisplayName = "None"),
	Hovered      UMETA(DisplayName = "Hovered"),
	Selected     UMETA(DisplayName = "Selected"),
	Neighbor     UMETA(DisplayName = "Neighbor"),
	IsInPath     UMETA(DisplayName = "Is In Path"),
	IsDiscovered UMETA(DisplayName = "Is Discovered"),
	IsAnalyzed   UMETA(DisplayName = "Is Analyzed"),
	IsReachable  UMETA(DisplayName = "Is Reachable"),
	IsInRange    UMETA(DisplayName = "Is In Range"),
	IsSpellAOE   UMETA(DisplayName = "Is Spell AOE")
};
