#pragma once

#include "CoreMinimal.h"
#include "GAS/MyBaseGameplayAbility.h"
#include "BasicRangedAttackAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API UBasicRangedAttackAbility : public UMyBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UBasicRangedAttackAbility();

	// Damage properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 BaseDamage = 6; // 1d6 base weapon damage

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 DexterityModifier = 3; // Will be dynamic later does not affect damage

	// Attack roll properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 AttackBonus = 5; // Base attack bonus

	// Ranged specific properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 MaxRange = 30; // Maximum range in squares

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 LongRangeThreshold = 10; // Range where penalties start

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Helper functions
	UFUNCTION(BlueprintCallable, Category = "Combat")
	int32 RollAttack(int32 RangePenalty = 0) const;

	UFUNCTION(BlueprintCallable, Category = "Combat") 
	int32 RollDamage() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsTargetInRange(class ACombatant* Target, int32& OutRangePenalty) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool HasLineOfSight(class ACombatant* Target) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAttackHit(class ACombatant* Target, int32 Damage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAttackMiss(class ACombatant* Target);
};