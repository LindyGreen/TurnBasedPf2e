#pragma once

#include "CoreMinimal.h"
#include "ESpellArea.generated.h"

UENUM(BlueprintType)
enum class ESpellArea : uint8
{
	SingleTarget	UMETA(DisplayName = "Single Target"),
	MultipleTargets	UMETA(DisplayName = "Multiple Targets"),
	Line			UMETA(DisplayName = "Line"),
	Cone			UMETA(DisplayName = "Cone"),
	Burst			UMETA(DisplayName = "Burst"),
	SelfOrEmanation	UMETA(DisplayName = "SelfOrEmanation")
};