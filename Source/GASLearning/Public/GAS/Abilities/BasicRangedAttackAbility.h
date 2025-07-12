#pragma once

#include "CoreMinimal.h"
#include "GAS/MyBaseGameplayAbility.h"
#include "GAS/PF2eCombatLibrary.h"
#include "BasicRangedAttackAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API UBasicRangedAttackAbility : public UMyBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UBasicRangedAttackAbility();

	// Ranged specific properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 MaxRange = 30; // Maximum range in squares

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 LongRangeThreshold = 10; // Range where penalties start

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Helper functions
	UFUNCTION(BlueprintCallable, Category = "Combat")
	EDegreeOfSuccess RollAttack(int32 RangePenalty, int32 TargetAC) const;

	UFUNCTION(BlueprintCallable, Category = "Combat") 
	int32 RollDamage() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsTargetInRange(class ACombatant* Target, int32& OutRangePenalty) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")//probably redundant will be handled by the pre-attack phase  
	bool HasLineOfSight(class ACombatant* Target) const;

	// PF2e degree of success events ON THE TARGET
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAttackCriticalSuccess(class ACombatant* Target, int32 Damage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAttackSuccess(class ACombatant* Target, int32 Damage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAttackFailure(class ACombatant* Target);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAttackCriticalFailure(class ACombatant* Target);
};