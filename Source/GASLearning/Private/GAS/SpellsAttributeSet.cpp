#include "GAS/SpellsAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

USpellsAttributeSet::USpellsAttributeSet()
{
	// Initialize all spell resources to 0 (no spellcasting ability by default)
	InitSpellAttackBonus(0.0f);
	InitSpellSaveDC(10.0f); // Base DC is usually 10
	InitIsSpontaneous(0.0f); // 0 = Prepared, 1 = Spontaneous
	InitPreparableCantrips(0.0f);
	
	// Level 1 slots
	InitLevel1Slots(0.0f);
	InitLevel1Slot1(0.0f);
	InitLevel1Slot2(0.0f);
	InitLevel1Slot3(0.0f);
	
	// Level 2 slots
	InitLevel2Slots(0.0f);
	InitLevel2Slot1(0.0f);
	InitLevel2Slot2(0.0f);
	InitLevel2Slot3(0.0f);
	
	// Level 3 slots
	InitLevel3Slots(0.0f);
	InitLevel3Slot1(0.0f);
	InitLevel3Slot2(0.0f);
	InitLevel3Slot3(0.0f);
	
	// Divine/Focus
	InitDivineFont(0.0f);
	InitMaxFocusPoints(0.0f);
	InitCurrentFocusPoints(0.0f);
}


void USpellsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayAttribute ModifiedAttribute = Data.EvaluatedData.Attribute;
	float Delta = Data.EvaluatedData.Magnitude;

	// Broadcast delegates for commonly changed attributes (for UI updates)
	if (ModifiedAttribute == GetSpellAttackBonusAttribute())
	{
		OnSpellAttackBonusChanged.Broadcast(Delta, GetSpellAttackBonus());
	}
	else if (ModifiedAttribute == GetSpellSaveDCAttribute())
	{
		OnSpellSaveDCChanged.Broadcast(Delta, GetSpellSaveDC());
	}
	else if (ModifiedAttribute == GetLevel1SlotsAttribute())
	{
		OnLevel1SlotsChanged.Broadcast(Delta, GetLevel1Slots());
	}
	else if (ModifiedAttribute == GetLevel2SlotsAttribute())
	{
		OnLevel2SlotsChanged.Broadcast(Delta, GetLevel2Slots());
	}
	else if (ModifiedAttribute == GetLevel3SlotsAttribute())
	{
		OnLevel3SlotsChanged.Broadcast(Delta, GetLevel3Slots());
	}
	else if (ModifiedAttribute == GetCurrentFocusPointsAttribute())
	{
		OnCurrentFocusPointsChanged.Broadcast(Delta, GetCurrentFocusPoints());
	}
	
	// Clamp focus points between 0 and max
	if (ModifiedAttribute == GetCurrentFocusPointsAttribute())
	{
		SetCurrentFocusPoints(FMath::Clamp(GetCurrentFocusPoints(), 0.0f, GetMaxFocusPoints()));
	}
}