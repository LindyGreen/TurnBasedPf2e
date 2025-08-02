#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SkillsAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillChanged, float, Magnitude, float, NewValue);

/**
 * Attribute Set for PF2e Skills
 * Contains all 16 core skills from Pathfinder 2e
 */
UCLASS()
class GASLEARNING_API USkillsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	USkillsAttributeSet();

	// Core PF2e Skills
	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Acrobatics;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Acrobatics)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Arcana;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Arcana)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Athletics;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Athletics)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Crafting;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Crafting)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Deception;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Deception)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Diplomacy;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Diplomacy)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Intimidation;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Intimidation)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Medicine;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Medicine)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Nature;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Nature)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Occultism;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Occultism)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Performance;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Performance)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Religion;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Religion)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Society;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Society)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Stealth;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Stealth)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Survival;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Survival)

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	FGameplayAttributeData Thievery;
	ATTRIBUTE_ACCESSORS(USkillsAttributeSet, Thievery)

	// Delegate for UI updates if I ever use the skills in UI. Likely not
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnAcrobaticsChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnArcanaChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnAthleticsChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnCraftingChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnDeceptionChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnDiplomacyChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnIntimidationChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnMedicineChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnNatureChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnOccultismChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnPerformanceChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnReligionChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnSocietyChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnStealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnSurvivalChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSkillChanged OnThieveryChanged;

	// Attribute change handlers
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

};