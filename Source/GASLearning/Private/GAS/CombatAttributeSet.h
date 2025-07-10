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
	
	
};
