#pragma once

#include "CoreMinimal.h"
#include "FS_CombatAttributes.generated.h"

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_CombatAttributes
{
	GENERATED_BODY()

public:
	// Health attributes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Max Health"))
	float MaxHealth;

	// Combat attributes  
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Armor Class"))
	float AC;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Fortitude Save"))
	float Fortitude;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Reflex Save"))
	float Reflex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Will Save"))
	float Will;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Perception"))
	float Perception;

	// Movement
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Movement Speed"))
	float MovementSpeed;

	// Actions
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Max Actions Per Turn"))
	float MaxActions;

	// Attack modifiers
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Attack Bonus"))
	int32 AttackBonus;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Damage Bonus"))
	int32 DamageBonus;

	FS_CombatAttributes()
	{
		// PF2e reasonable defaults
		MaxHealth = 50.0f;
		AC = 15.0f;
		Fortitude = 3.0f;
		Reflex = 2.0f;
		Will = 1.0f;
		Perception = 2.0f;
		MovementSpeed = 30.0f;
		MaxActions = 3.0f;
		AttackBonus = 5;
		DamageBonus = 3;
	}
};