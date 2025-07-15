#pragma once

#include "CoreMinimal.h"
#include "ECombatAttributeType.generated.h"

UENUM(BlueprintType)
enum class ECombatAttributeType : uint8
{
	Health              UMETA(DisplayName = "Health"),
	MaxHealth           UMETA(DisplayName = "Max Health"),
	AC                  UMETA(DisplayName = "Armor Class"),
	Fortitude           UMETA(DisplayName = "Fortitude Save"),
	Reflex              UMETA(DisplayName = "Reflex Save"),
	Will                UMETA(DisplayName = "Will Save"),
	Perception          UMETA(DisplayName = "Perception"),
	MovementSpeed       UMETA(DisplayName = "Movement Speed"),
	Initiative          UMETA(DisplayName = "Initiative"),
	ActionsRemaining    UMETA(DisplayName = "Actions Remaining"),
	MaxActions          UMETA(DisplayName = "Max Actions"),
	ReactionAvailable   UMETA(DisplayName = "Reaction Available"),
	AttackBonus         UMETA(DisplayName = "Attack Bonus"),
	DamageBonus         UMETA(DisplayName = "Damage Bonus"),
	DamageDie           UMETA(DisplayName = "Damage Die"),
	DamageDieCount      UMETA(DisplayName = "Damage Die Count"),
	MaxDieRoll          UMETA(DisplayName = "Max Die Roll")
};