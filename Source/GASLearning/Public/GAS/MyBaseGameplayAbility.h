#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "StructsAndEnums/EAbilityVariantType.h"
#include "StructsAndEnums/EAbilityCategory.h"
#include "StructsAndEnums/ESpellArea.h"
#include "StructsAndEnums/E_TileState.h"
#include "RangeFinderLibrary.h"
#include "GAS/PF2eCombatLibrary.h"
#include "MyBaseGameplayAbility.generated.h"
class AGrid;
class UTexture2d;
class AController_TurnBased;
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

	// Cached ASC reference
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TObjectPtr<UAbilitySystemComponent> CachedASC;
	
	// Cached RangeFinder reference
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TObjectPtr<class URangeFinder> RangeFinderRef;
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

	// Spell properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 SpellLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	ESpellArea AreaType = ESpellArea::SingleTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 AreaSize = 1; // Radius for burst/emanation, length for line/cone

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 MaxTargets = 1; // Maximum number of targets for MultipleTarget spells

	// Damage properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	FGameplayTag DamageType; // Damage type like "Damage.Physical.Slashing", onle 1 per abiltiy for simplicity
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 BaseDamage = 6; // Base damage die (for spells)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 SaveDC = 15; // Spell save DC
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 DamageDiceCount = 2; // Number of dice (for spells, e.g., 2d6)
	
	// Weapon ability modifiers
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Martial")
	int32 BonusDamageDice = 0; // Extra damage dice (e.g., Power Attack)
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Martial")
	int32 BonusDamageFlat = 0; // Flat damage bonus
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Martial")
	bool bCanSneakAttack = false; // Can this ability sneak attack?
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Martial")
	bool bIsAgile = false;
	
	// Weapon traits
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Traits")
	bool bHasDeadlyTrait = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Traits", meta = (EditCondition = "bHasDeadlyTrait"))
	int32 DeadlyDieSize = 6; // d6, d8, d10, d12

	// Targeting system
	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	bool bIsTargeting = false;

	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	FIntPoint CurrentHoveredTile = FIntPoint(-1, -1);
	
	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	TArray<FIntPoint> TargetedTiles;

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

protected:
	// Override to initialize references when ability is granted to ASC
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Setup")
	int32 GetRange() const { return Range; }
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void CancelAllOtherActiveAbilities();

	// Universal combat functions
	UFUNCTION(BlueprintCallable, Category = "Combat")
	float RollAbilityDamage() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	EDegreeOfSuccess RollAbilityAttack(AActor* Target, float RangePenalty = 0) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	EDegreeOfSuccess RollSavingThrow(int32 TargetSaveBonus = 5) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	TArray<FIntPoint> GetAbilityArea(FIntPoint TargetLocation) const;

	// Range and targeting utilities

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool HasLineOfSight(class ACombatant* Target) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	int32 CalculateDistanceToTarget(class ACombatant* Target) const;
	
	// Look-at system for targeting
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void EnableCombatantLookAt();
	
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void DisableCombatantLookAt();
	// Look-at system variables
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TObjectPtr<AController_TurnBased> PlayerControllerRef = nullptr;
	
	// Delegate callback for tile hover changes
	UFUNCTION()
	void OnTileHoverChanged(FIntPoint NewTileIndex);

	// Multiple Attack Penalty system
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyMAPTags() const;

	// New damage system using ExecutionCalculation
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyDamageToTarget(class ACombatant* Target, float DamageAmount, bool bIsCriticalHit = false);
	
	// Aiming/Targeting system
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void StartAimingStage();
	
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void GenerateAffectedTilesForAIEAndSingleSpells(FIntPoint OriginPoint);

	// Reference to the damage GameplayEffect class (set in Blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<class UGameplayEffect> DamageGameplayEffectClass;

	// Cached target AC for attack rolls (set by implementation)
	mutable float TargetAC = 10;
};