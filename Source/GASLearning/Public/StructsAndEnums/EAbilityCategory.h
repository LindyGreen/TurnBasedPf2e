#pragma once

#include "CoreMinimal.h"
#include "EAbilityCategory.generated.h"

UENUM(BlueprintType)
enum class EAbilityCategory : uint8
{
	Movement		UMETA(DisplayName = "Movement"),
	Attack			UMETA(DisplayName = "Attack"),
	SpellAttacking	UMETA(DisplayName = "SpellAttacking"),
	SpellSave		UMETA(DisplayName = "SpellSaveRequiring"),
	Item			UMETA(DisplayName = "Item"),
	FreeAction		UMETA(DisplayName = "Free Action"),
	Reaction		UMETA(DisplayName = "Reaction")
};