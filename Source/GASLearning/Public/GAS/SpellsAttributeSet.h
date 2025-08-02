#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SpellsAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellResourceChanged, float, Magnitude, float, NewValue);

/**
 * Attribute Set for PF2e Spellcasting Resources
 * Contains spell slots, focus points, and spellcasting attributes
 */
UCLASS()
class GASLEARNING_API USpellsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	USpellsAttributeSet();

	// Spell Attack and Save DC
	UPROPERTY(BlueprintReadOnly, Category = "Spellcasting")
	FGameplayAttributeData SpellAttackBonus;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, SpellAttackBonus)

	UPROPERTY(BlueprintReadOnly, Category = "Spellcasting")
	FGameplayAttributeData SpellSaveDC;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, SpellSaveDC)

	// Casting Type (0 = Prepared, 1 = Spontaneous)
	UPROPERTY(BlueprintReadOnly, Category = "Spellcasting")
	FGameplayAttributeData IsSpontaneous;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, IsSpontaneous)

	// Cantrips
	UPROPERTY(BlueprintReadOnly, Category = "Cantrips")
	FGameplayAttributeData PreparableCantrips;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, PreparableCantrips)

	// Level 1 Spell Slots (for spontaneous casters)
	UPROPERTY(BlueprintReadOnly, Category = "Level 1 Spells")
	FGameplayAttributeData Level1Slots;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level1Slots)

	// Level 1 Individual Slots (for prepared casters - 0/1 values)
	UPROPERTY(BlueprintReadOnly, Category = "Level 1 Spells")
	FGameplayAttributeData Level1Slot1;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level1Slot1)

	UPROPERTY(BlueprintReadOnly, Category = "Level 1 Spells")
	FGameplayAttributeData Level1Slot2;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level1Slot2)

	UPROPERTY(BlueprintReadOnly, Category = "Level 1 Spells")
	FGameplayAttributeData Level1Slot3;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level1Slot3)

	// Level 2 Spell Slots
	UPROPERTY(BlueprintReadOnly, Category = "Level 2 Spells")
	FGameplayAttributeData Level2Slots;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level2Slots)

	UPROPERTY(BlueprintReadOnly, Category = "Level 2 Spells")
	FGameplayAttributeData Level2Slot1;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level2Slot1)

	UPROPERTY(BlueprintReadOnly, Category = "Level 2 Spells")
	FGameplayAttributeData Level2Slot2;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level2Slot2)

	UPROPERTY(BlueprintReadOnly, Category = "Level 2 Spells")
	FGameplayAttributeData Level2Slot3;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level2Slot3)

	// Level 3 Spell Slots
	UPROPERTY(BlueprintReadOnly, Category = "Level 3 Spells")
	FGameplayAttributeData Level3Slots;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level3Slots)

	UPROPERTY(BlueprintReadOnly, Category = "Level 3 Spells")
	FGameplayAttributeData Level3Slot1;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level3Slot1)

	UPROPERTY(BlueprintReadOnly, Category = "Level 3 Spells")
	FGameplayAttributeData Level3Slot2;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level3Slot2)

	UPROPERTY(BlueprintReadOnly, Category = "Level 3 Spells")
	FGameplayAttributeData Level3Slot3;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Level3Slot3)

	// Divine Font (for clerics/champions)
	UPROPERTY(BlueprintReadOnly, Category = "Divine Magic")
	FGameplayAttributeData DivineFont;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, DivineFont)

	// Focus Points
	UPROPERTY(BlueprintReadOnly, Category = "Focus Magic")
	FGameplayAttributeData MaxFocusPoints;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, MaxFocusPoints)

	UPROPERTY(BlueprintReadOnly, Category = "Focus Magic")
	FGameplayAttributeData CurrentFocusPoints;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, CurrentFocusPoints)

	// Delegates for UI updates - only the most commonly changed ones
	UPROPERTY(BlueprintAssignable)
	FOnSpellResourceChanged OnSpellAttackBonusChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSpellResourceChanged OnSpellSaveDCChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSpellResourceChanged OnLevel1SlotsChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSpellResourceChanged OnLevel2SlotsChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSpellResourceChanged OnLevel3SlotsChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSpellResourceChanged OnCurrentFocusPointsChanged;

protected:

	// Attribute change handlers
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};