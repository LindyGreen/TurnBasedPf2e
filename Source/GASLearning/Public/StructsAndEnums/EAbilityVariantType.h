#pragma once

#include "CoreMinimal.h"
#include "EAbilityVariantType.generated.h"

UENUM(BlueprintType)
enum class EAbilityVariantType : uint8
{
	OneAction		UMETA(DisplayName = "1 Action"),
	TwoAction		UMETA(DisplayName = "2 Actions"),
	ThreeAction		UMETA(DisplayName = "3 Actions"),
	MeleeMode		UMETA(DisplayName = "Melee"),
	RangedMode		UMETA(DisplayName = "Ranged")
};