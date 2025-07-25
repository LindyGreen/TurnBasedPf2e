#pragma once

#include "CoreMinimal.h"
#include "ESpellArea.generated.h"

UENUM(BlueprintType)
enum class ESpellArea : uint8
{
	Single			UMETA(DisplayName = "Single Target"),
	MultipleTarger  UMETA(DisplayName = "MultipleTarget"),
	Line			UMETA(DisplayName = "Line"),
	Cone			UMETA(DisplayName = "Cone"),
	Burst			UMETA(DisplayName = "Burst"),
	Emanation		UMETA(DisplayName = "Emanation")
};