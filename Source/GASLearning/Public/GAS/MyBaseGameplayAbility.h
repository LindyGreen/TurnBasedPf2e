#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/Texture2D.h"
#include "StructsAndEnums/EAbilityVariantType.h"
#include "StructsAndEnums/EAbilityCategory.h"
#include "MyBaseGameplayAbility.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class GASLEARNING_API UMyBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMyBaseGameplayAbility();

	// UI Data for ability widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	EAbilityCategory Category = EAbilityCategory::Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 ActionCost = 1;

	// Additional PF2e specific properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 Range = 5; // in squares

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bRequiresTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanTargetSelf = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanTargetAllies = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanTargetEnemies = true;

	// Set to true for abilities that are part of a composite ability (like movement in Sudden Charge)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsSubAbility = false;

	// Ability Variants System
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Variants")
	bool bHasVariants = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Variants", meta = (EditCondition = "bHasVariants"))
	TArray<EAbilityVariantType> SupportedVariants;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	EAbilityVariantType SelectedVariant = EAbilityVariantType::OneAction;

	// Blueprint-callable getters for UI
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	FText GetAbilityDisplayName() const { return DisplayName; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	FText GetAbilityDescription() const { return Description; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	UTexture2D* GetAbilityIcon() const { return Icon; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	EAbilityCategory GetAbilityCategory() const { return Category; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
	int32 GetActionCost() const { return ActionCost; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
	int32 GetRange() const { return Range; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void OnAbilityActivationSuccess();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void OnAbilityActivationFailed();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	// Helper function to spend actions
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TrySpendActions(int32 ActionsToSpend = 1);

	// Helper function to get the combatant
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
	class ACombatant* GetOwningCombatant() const;

	// Helper function to cancel all other active abilities
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CancelAllOtherActiveAbilities();

private:
	// Internal helper function (not exposed to Blueprint)
	class ACombatant* GetCombatantFromActorInfo(const FGameplayAbilityActorInfo* ActorInfo) const;
};