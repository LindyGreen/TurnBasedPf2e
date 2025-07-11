// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/BaseAttributeSet.h"
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
	UPROPERTY()
	FGameplayAttributeData Health;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Health);
	UPROPERTY()
	FGameplayAttributeData MaxHealth;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxHealth);
	// Perception (no broadcast)
	UPROPERTY()
	FGameplayAttributeData Perception;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet,
	Perception);
#pragma region SavesAndAC
	// AC (broadcast only)
	UPROPERTY()
	FGameplayAttributeData AC;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, AC);

	UPROPERTY()
	FPlayAttributeEvent OnACChanged;

	// Saves (broadcast only)
	
	UPROPERTY()
	FGameplayAttributeData Fortitude;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet,
	Fortitude);

	UPROPERTY()
	FPlayAttributeEvent OnFortitudeChanged;

	UPROPERTY()
	FGameplayAttributeData Reflex;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Reflex);

	UPROPERTY()
	FPlayAttributeEvent OnReflexChanged;

	UPROPERTY()
	FGameplayAttributeData Will;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Will);

	UPROPERTY()
	FPlayAttributeEvent OnWillChanged;

#pragma endregion 

#pragma region Movement
	// Movement Speed (no broadcast required)
	UPROPERTY()
	FGameplayAttributeData MovementSpeed;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MovementSpeed);

#pragma endregion 

#pragma region Initiative
	// Initiative (broadcast for turn order changes)
	UPROPERTY()
	FGameplayAttributeData Initiative;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Initiative);
#pragma endregion

#pragma region Actions
	// Actions remaining this turn (broadcast for UI/AI updates)
	UPROPERTY()
	FGameplayAttributeData ActionsRemaining;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ActionsRemaining);
	
	UPROPERTY()
	FPlayAttributeEvent OnActionsRemainingChanged;
	
	// Max actions per turn (usually 3, modified by conditions)
	UPROPERTY()
	FGameplayAttributeData MaxActions;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxActions);
	
	// Reaction available (0 or 1)
	UPROPERTY()
	FGameplayAttributeData ReactionAvailable;
	PLAY_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ReactionAvailable);
	
	UPROPERTY()
	FPlayAttributeEvent OnReactionAvailableChanged;

#pragma endregion
	
	
};
