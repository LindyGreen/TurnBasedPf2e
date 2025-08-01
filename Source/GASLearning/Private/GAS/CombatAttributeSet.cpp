// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CombatAttributeSet.h"
#include "LogTypes.h"
#include "GameplayEffectExtension.h"

/*UCombatAttributeSet::UCombatAttributeSet()
{
//WELL would you look at that,
//I need to figure how to make this work with my DT, maybe well, who knows	
}*/

void UCombatAttributeSet::PostGameplayEffectExecute(
	const struct FGameplayEffectModCallbackData& Data)
{//EXECUTED ON ATTRIBUTE CHANGE

	//HP
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));//ClampHealth 
		OnHealthChanged.Broadcast(Data.EvaluatedData.Magnitude, GetHealth());//DefaultBroadcasterForHealth
	}
	//SAVES AND AC
	else if (Data.EvaluatedData.Attribute ==GetACAttribute())
	{

		OnACChanged.Broadcast(Data.EvaluatedData.Magnitude,GetAC());
	}
	else if (Data.EvaluatedData.Attribute ==GetFortitudeAttribute())
	{
		OnFortitudeChanged.Broadcast(Data.EvaluatedData.Magnitude, GetFortitude());
	}
	else if (Data.EvaluatedData.Attribute ==GetReflexAttribute())
	{
		OnReflexChanged.Broadcast(Data.EvaluatedData.Magnitude, GetReflex());
	}
	else if (Data.EvaluatedData.Attribute ==GetWillAttribute())
	{

		OnWillChanged.Broadcast(Data.EvaluatedData.Magnitude,GetWill());
	}
	//ACTIONS
	else if (Data.EvaluatedData.Attribute == GetActionsRemainingAttribute())
	{
		float ClampedValue = FMath::Clamp(GetActionsRemaining(), 0.0f, GetMaxActions());
		SetActionsRemaining(ClampedValue);
		OnActionsRemainingChanged.Broadcast(Data.EvaluatedData.Magnitude, ClampedValue);
	}
	else if (Data.EvaluatedData.Attribute == GetReactionAvailableAttribute())
	{
		SetReactionAvailable(FMath::Clamp(GetReactionAvailable(), 0, 1));
		OnReactionAvailableChanged.Broadcast(Data.EvaluatedData.Magnitude, GetReactionAvailable());
	}
	//META ATTRIBUTES
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		// IncomingDamage Meta Attribute - process and apply to Health
		const float DamageAmount = GetIncomingDamage();
		
		// Apply damage to health (could subtract from shields first if we had them)
		const float NewHealth = FMath::Max(0.0f, GetHealth() - DamageAmount);
		SetHealth(NewHealth);
		
		// Reset the meta attribute
		SetIncomingDamage(0.0f);
		
		// Broadcast health change
		OnHealthChanged.Broadcast(-DamageAmount, NewHealth);
		
		UE_LOG(Log_Combat, Log, TEXT("Applied %f damage. Health: %f -> %f"), 
			DamageAmount, GetHealth() + DamageAmount, NewHealth);
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingHealingAttribute())
	{
		// IncomingHealing Meta Attribute - process and apply to Health
		const float HealAmount = GetIncomingHealing();
		
		// Apply healing to health (clamped to MaxHealth)
		const float NewHealth = FMath::Min(GetMaxHealth(), GetHealth() + HealAmount);
		SetHealth(NewHealth);
		
		// Reset the meta attribute
		SetIncomingHealing(0.0f);
		
		// Broadcast health change
		OnHealthChanged.Broadcast(HealAmount, NewHealth);
		
		UE_LOG(Log_Combat, Log, TEXT("Applied %f healing. Health: %f -> %f"), 
			HealAmount, GetHealth() - HealAmount, NewHealth);
	}

}
