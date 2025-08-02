#include "GAS/SkillsAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

USkillsAttributeSet::USkillsAttributeSet()
{
	// Initialize all skills to 0 (untrained)
	InitAcrobatics(0.0f);
	InitArcana(0.0f);
	InitAthletics(0.0f);
	InitCrafting(0.0f);
	InitDeception(0.0f);
	InitDiplomacy(0.0f);
	InitIntimidation(0.0f);
	InitMedicine(0.0f);
	InitNature(0.0f);
	InitOccultism(0.0f);
	InitPerformance(0.0f);
	InitReligion(0.0f);
	InitSociety(0.0f);
	InitStealth(0.0f);
	InitSurvival(0.0f);
	InitThievery(0.0f);
}


void USkillsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayAttribute ModifiedAttribute = Data.EvaluatedData.Attribute;
	float Delta = Data.EvaluatedData.Magnitude;

	// Broadcast delegates for UI updates
	if (ModifiedAttribute == GetAcrobaticsAttribute())
	{
		OnAcrobaticsChanged.Broadcast(Delta, GetAcrobatics());
	}
	else if (ModifiedAttribute == GetArcanaAttribute())
	{
		OnArcanaChanged.Broadcast(Delta, GetArcana());
	}
	else if (ModifiedAttribute == GetAthleticsAttribute())
	{
		OnAthleticsChanged.Broadcast(Delta, GetAthletics());
	}
	else if (ModifiedAttribute == GetCraftingAttribute())
	{
		OnCraftingChanged.Broadcast(Delta, GetCrafting());
	}
	else if (ModifiedAttribute == GetDeceptionAttribute())
	{
		OnDeceptionChanged.Broadcast(Delta, GetDeception());
	}
	else if (ModifiedAttribute == GetDiplomacyAttribute())
	{
		OnDiplomacyChanged.Broadcast(Delta, GetDiplomacy());
	}
	else if (ModifiedAttribute == GetIntimidationAttribute())
	{
		OnIntimidationChanged.Broadcast(Delta, GetIntimidation());
	}
	else if (ModifiedAttribute == GetMedicineAttribute())
	{
		OnMedicineChanged.Broadcast(Delta, GetMedicine());
	}
	else if (ModifiedAttribute == GetNatureAttribute())
	{
		OnNatureChanged.Broadcast(Delta, GetNature());
	}
	else if (ModifiedAttribute == GetOccultismAttribute())
	{
		OnOccultismChanged.Broadcast(Delta, GetOccultism());
	}
	else if (ModifiedAttribute == GetPerformanceAttribute())
	{
		OnPerformanceChanged.Broadcast(Delta, GetPerformance());
	}
	else if (ModifiedAttribute == GetReligionAttribute())
	{
		OnReligionChanged.Broadcast(Delta, GetReligion());
	}
	else if (ModifiedAttribute == GetSocietyAttribute())
	{
		OnSocietyChanged.Broadcast(Delta, GetSociety());
	}
	else if (ModifiedAttribute == GetStealthAttribute())
	{
		OnStealthChanged.Broadcast(Delta, GetStealth());
	}
	else if (ModifiedAttribute == GetSurvivalAttribute())
	{
		OnSurvivalChanged.Broadcast(Delta, GetSurvival());
	}
	else if (ModifiedAttribute == GetThieveryAttribute())
	{
		OnThieveryChanged.Broadcast(Delta, GetThievery());
	}
}