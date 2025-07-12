#pragma once

#include "CoreMinimal.h"
#include "GAS/MyBaseGameplayAbility.h"
#include "GAS/PF2eCombatLibrary.h"
#include "BasicMeleeAttackAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API UBasicMeleeAttackAbility : public UMyBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UBasicMeleeAttackAbility();

	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Helper functions
	UFUNCTION(BlueprintCallable, Category = "Combat")
	EDegreeOfSuccess RollAttack(int32 TargetAC = 15) const;

	UFUNCTION(BlueprintCallable, Category = "Combat") 
	int32 RollDamage() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsTargetInRange(class ACombatant* Target) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAttackCriticalSuccess(class ACombatant* Target, int32 DoubleDamage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAttackSuccess(class ACombatant* Target, int32 Damage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAttackFailure(class ACombatant* Target);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAttackCriticalFailure(class ACombatant* Target);
};