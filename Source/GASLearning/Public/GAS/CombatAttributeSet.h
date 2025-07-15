// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/BaseAttributeSet.h"//Its attributeSet.h child with the macro defined in it
#include "AbilitySystemComponent.h"
#include "CombatAttributeSet.generated.h"


/**
 * 
 */
UCLASS()
class GASLEARNING_API UCombatAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()
public:
	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	UPROPERTY()
	FPlayAttributeEvent OnHealthChanged;
	//Health (clamp and broadcast)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Health);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxHealth);
	// Perception (no broadcast)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Perception;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet,
	Perception);
#pragma region SavesAndAC
	// AC (broadcast only)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData AC;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, AC);

	UPROPERTY()
	FPlayAttributeEvent OnACChanged;

	// Saves (broadcast only)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Fortitude;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet,
	Fortitude);

	UPROPERTY()
	FPlayAttributeEvent OnFortitudeChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Reflex;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Reflex);

	UPROPERTY()
	FPlayAttributeEvent OnReflexChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Will;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Will);

	UPROPERTY()
	FPlayAttributeEvent OnWillChanged;

#pragma endregion 

#pragma region Movement
	// Movement Speed (no broadcast required)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MovementSpeed);

#pragma endregion 

#pragma region Initiative
	// Initiative (broadcast for turn order changes)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Initiative;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Initiative);
#pragma endregion

#pragma region Actions
	// Actions remaining this turn (broadcast for UI/AI updates)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData ActionsRemaining;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ActionsRemaining);
	
	UPROPERTY()
	FPlayAttributeEvent OnActionsRemainingChanged;
	
	// Max actions per turn (usually 3, modified by conditions)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxActions;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxActions);
	
	// Reaction available (0 or 1)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData ReactionAvailable;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ReactionAvailable);
	
	UPROPERTY()
	FPlayAttributeEvent OnReactionAvailableChanged;

#pragma endregion

#pragma region AttackAndDamage
	// Attack modifiers for abilities to use
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData AttackBonus;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, AttackBonus);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData DamageBonus;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, DamageBonus);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData DamageDie;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, DamageDie);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData DamageDieCount;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, DamageDieCount);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxDieRoll;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxDieRoll);

#pragma endregion AttackAndDamage
};