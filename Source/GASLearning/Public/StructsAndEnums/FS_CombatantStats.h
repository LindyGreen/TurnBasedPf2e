#pragma once

#include "CoreMinimal.h"
#include "FS_CombatAttributes.h"
#include "FS_CharacterAbilities.h"
#include "E_TileType.h"
#include "FS_CombatantStats.generated.h"

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_CombatantStats
{
	GENERATED_BODY()

public:
	// Combat stats and attributes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Combat Attributes"))
	FS_CombatAttributes CombatAttributes;

	// Abilities and effects
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Character Abilities"))
	FS_CharacterAbilities CharacterAbilities;

	// Movement/pathfinding constraints
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Accessible Tiles"))
	TArray<ETileType> AccessibleTiles;

	FS_CombatantStats()
	{
		CombatAttributes = FS_CombatAttributes();
		CharacterAbilities = FS_CharacterAbilities();
		AccessibleTiles = TArray<ETileType>();
	}
};