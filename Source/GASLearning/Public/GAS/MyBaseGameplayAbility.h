#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "StructsAndEnums/EAbilityVariantType.h"
#include "StructsAndEnums/EAbilityCategory.h"
#include "StructsAndEnums/ESpellArea.h"
#include "GAS/PF2eCombatLibrary.h"
#include "MyBaseGameplayAbility.generated.h"
class AGrid;
class UTexture2d;
UCLASS(Abstract, BlueprintType, Blueprintable)
class GASLEARNING_API UMyBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMyBaseGameplayAbility();
	//GridRef for ability highlighting
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TObjectPtr<AGrid> GridRef;

	// Cached owning combatant reference
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TObjectPtr<class ACombatant> OwningCombatant;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsInstant = false;

	// Spell properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 SpellLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	ESpellArea AreaType = ESpellArea::Single;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 AreaSize = 1; // Radius for burst/emanation, length for line/cone

	// Damage properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 BaseDamage = 6; // Base damage die

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 DamageDiceCount = 2; // Number of dice (e.g., 2d6)

	// Saving throw properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	bool bAllowsSave = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 SaveDC = 15; // Spell save DC

	// Ranged attack properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 MaxRange = 30; // Maximum range in squares

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 LongRangeThreshold = 10; // Range where penalties start

	// Targeting system
	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	bool bIsTargeting = false;

	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	FIntPoint CurrentHoveredTile = FIntPoint(-1, -1);

	// Animation montages
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSoftObjectPtr<class UAnimMontage> PreActivationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSoftObjectPtr<class UAnimMontage> PostActivationMontage;

	// Ability Variants System
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Variants")
	bool bHasVariants = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Variants", meta = (EditCondition = "bHasVariants"))
	TArray<EAbilityVariantType> SupportedVariants;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	EAbilityVariantType SelectedVariant = EAbilityVariantType::OneAction;

	// Blueprint-callable getters for UI
#pragma region Inlined Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	FText GetAbilityDisplayName() const { return DisplayName; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	FText GetAbilityDescription() const { return Description; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	UTexture2D* GetAbilityIcon() const { return Icon; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	EAbilityCategory GetAbilityCategory() const { return Category; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Setup")
	int32 GetActionCost() const { return ActionCost; }
#pragma endregion Inlined Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Setup")
	int32 GetRange() const { return Range; }
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void CancelAllOtherActiveAbilities();

	// Universal combat functions
	UFUNCTION(BlueprintCallable, Category = "Combat")
	int32 RollDamage() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	EDegreeOfSuccess RollAttack(int32 TargetAC, int32 RangePenalty = 0) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	EDegreeOfSuccess RollSavingThrow(int32 TargetSaveBonus = 5) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	TArray<FIntPoint> GetAbilityArea(FIntPoint TargetLocation) const;

	// Range and targeting utilities
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsTargetInRange(class ACombatant* Target, int32& OutRangePenalty) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool HasLineOfSight(class ACombatant* Target) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	int32 CalculateDistanceToTarget(class ACombatant* Target) const;
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void SetCPPReferences();
};