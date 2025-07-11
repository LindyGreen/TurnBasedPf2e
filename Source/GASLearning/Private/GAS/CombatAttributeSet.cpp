// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatAttributeSet.h"
#include "GameplayEffectExtension.h"
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



}
