#pragma once

#include "CoreMinimal.h"
#include "EAbilityCategory.generated.h"

UENUM(BlueprintType)
enum class EAbilityCategory : uint8
{
	Utility			UMETA(DisplayName = "Utility"),
	Attack			UMETA(DisplayName = "Attack"),
	SpellAttack		UMETA(DisplayName = "Spell Attack"),
	Save			UMETA(DisplayName = "Save")
};