

#pragma once

#include "CoreMinimal.h"
#include "E_TileType.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
    None                UMETA(DisplayName = "None"),
    Normal              UMETA(DisplayName = "Normal"),
    Obstacle            UMETA(DisplayName = "Obstacle"),
    DifficultTerrain    UMETA(DisplayName = "Difficult Terrain"),
    GreaterDiffTerrain  UMETA(DisplayName = "Greater Difficult Terrain"),
    FlyOnly             UMETA(DisplayName = "Fly Only")
};
