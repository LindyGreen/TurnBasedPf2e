#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayTagContainer.h"
#include "PF2eDamageExecCalculation.generated.h"

/**
 * Execution Calculation for PF2e damage processing
 * Handles resistances, weaknesses, critical hits, deadly weapon traits, etc.
 */
UCLASS()
class GASLEARNING_API UPF2eDamageExecCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UPF2eDamageExecCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	// Helper functions for damage processing
	
	/** Get resistance value for a specific damage type from target's attributes */
	UFUNCTION()
	int32 GetResistanceValue(FGameplayTag DamageType, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const;
	
	/** Get weakness value for a specific damage type from target's attributes */
	UFUNCTION()
	int32 GetWeaknessValue(FGameplayTag DamageType, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const;
	
	
	/** Apply resistances and weaknesses to damage amount */
	UFUNCTION()
	float ApplyResistanceAndWeakness(float BaseDamage, FGameplayTag DamageType, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const;
	
	/** Check if this attack was a critical hit based on effect context or tags */
	UFUNCTION()
	bool IsCriticalHit(const FGameplayEffectSpec& Spec) const;
	
	/** Apply deadly weapon trait (extra damage dice on critical hits) */
	UFUNCTION()
	float ApplyDeadlyTrait(float BaseDamage, const FGameplayEffectSpec& Spec) const;
	
	/** Extract the damage type from the GameplayEffect spec */
	UFUNCTION()
	FGameplayTag GetDamageTypeFromSpec(const FGameplayEffectSpec& Spec) const;
};