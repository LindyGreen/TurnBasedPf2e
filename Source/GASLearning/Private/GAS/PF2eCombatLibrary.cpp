#include "GAS/PF2eCombatLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "LogTypes.h"

EDegreeOfSuccess UPF2eCombatLibrary::CalculateDegreeOfSuccess(int32 Roll, int32 DC, int32 D20Result)
{
	// First determine base degree of success:
	EDegreeOfSuccess BaseResult;
	if (Roll >= DC + 10)
	{
		BaseResult = EDegreeOfSuccess::CriticalSuccess;
	}
	else if (Roll >= DC)
	{
		BaseResult = EDegreeOfSuccess::Success;
	}
	else if (Roll >= DC - 10)
	{
		BaseResult = EDegreeOfSuccess::Failure;
	}
	else
	{
		BaseResult = EDegreeOfSuccess::CriticalFailure;
	}
	
	// Apply natural 20/1 modifiers
	return ApplyNaturalRollModifiers(BaseResult, D20Result);
}

EDegreeOfSuccess UPF2eCombatLibrary::RollAttack(int32 AttackBonus, int32 TargetAC, int32 MaxDieRoll)
{
	int32 D20Roll = RollD20(MaxDieRoll);
	int32 TotalRoll = D20Roll + AttackBonus;
	
	UE_LOG(LogGAS, Verbose, TEXT("PF2eCombatLibrary: Attack roll %d + %d = %d vs AC %d (MaxDie: %d)"), 
		D20Roll, AttackBonus, TotalRoll, TargetAC, MaxDieRoll);
	
	return CalculateDegreeOfSuccess(TotalRoll, TargetAC, D20Roll);
}

EDegreeOfSuccess UPF2eCombatLibrary::RollSave(int32 SaveBonus, int32 DC, int32 MaxDieRoll)
{
	int32 D20Roll = RollD20(MaxDieRoll);
	int32 TotalRoll = D20Roll + SaveBonus;
	
	UE_LOG(LogGAS, Verbose, TEXT("PF2eCombatLibrary: Save roll %d + %d = %d vs DC %d (MaxDie: %d)"), 
		D20Roll, SaveBonus, TotalRoll, DC, MaxDieRoll);
	
	return CalculateDegreeOfSuccess(TotalRoll, DC, D20Roll);
}

EDegreeOfSuccess UPF2eCombatLibrary::RollSkillCheck(int32 SkillBonus, int32 DC, int32 MaxDieRoll)
{
	int32 D20Roll = RollD20(MaxDieRoll);
	int32 TotalRoll = D20Roll + SkillBonus;
	
	UE_LOG(LogGAS, Verbose, TEXT("PF2eCombatLibrary: Skill check %d + %d = %d vs DC %d (MaxDie: %d)"), 
		D20Roll, SkillBonus, TotalRoll, DC, MaxDieRoll);
	
	return CalculateDegreeOfSuccess(TotalRoll, DC, D20Roll);
}

int32 UPF2eCombatLibrary::ApplyDamageMultiplier(int32 BaseDamage, EDegreeOfSuccess Result)
{
	switch (Result)
	{
		case EDegreeOfSuccess::CriticalSuccess:
			return 0; // No damage (for saves)
		case EDegreeOfSuccess::Success:
			return BaseDamage / 2; // Half damage (for saves) or normal damage (for attacks)
		case EDegreeOfSuccess::Failure:
			return BaseDamage; // Full damage
		case EDegreeOfSuccess::CriticalFailure:
			return BaseDamage * 2; // Double damage
		default:
			return BaseDamage;
	}
}

FString UPF2eCombatLibrary::GetDegreeOfSuccessString(EDegreeOfSuccess Degree)
{
	switch (Degree)
	{
		case EDegreeOfSuccess::CriticalSuccess:
			return TEXT("Critical Success");
		case EDegreeOfSuccess::Success:
			return TEXT("Success");
		case EDegreeOfSuccess::Failure:
			return TEXT("Failure");
		case EDegreeOfSuccess::CriticalFailure:
			return TEXT("Critical Failure");
		default:
			return TEXT("Unknown");
	}
}

EDegreeOfSuccess UPF2eCombatLibrary::ApplyNaturalRollModifiers(EDegreeOfSuccess BaseResult, int32 D20Result)
{
	// Apply natural 20/1 rules:
	// Natural 20: Improve result by one degree
	// Natural 1: Worsen result by one degree
	if (D20Result == 20)
	{
		// Improve by one degree (but can't go better than Critical Success)
		switch (BaseResult)
		{
			case EDegreeOfSuccess::CriticalFailure:
				return EDegreeOfSuccess::Failure;
			case EDegreeOfSuccess::Failure:
				return EDegreeOfSuccess::Success;
			case EDegreeOfSuccess::Success:
				return EDegreeOfSuccess::CriticalSuccess;
			case EDegreeOfSuccess::CriticalSuccess:
				return EDegreeOfSuccess::CriticalSuccess; // Already best
		}
	}
	else if (D20Result == 1)
	{
		// Worsen by one degree (but can't go worse than Critical Failure)
		switch (BaseResult)
		{
			case EDegreeOfSuccess::CriticalSuccess:
				return EDegreeOfSuccess::Success;
			case EDegreeOfSuccess::Success:
				return EDegreeOfSuccess::Failure;
			case EDegreeOfSuccess::Failure:
				return EDegreeOfSuccess::CriticalFailure;
			case EDegreeOfSuccess::CriticalFailure:
				return EDegreeOfSuccess::CriticalFailure; // Already worst
		}
	}
	
	return BaseResult;
}

int32 UPF2eCombatLibrary::RollDamage(int32 DamageDie, int32 DamageDieCount, int32 DamageBonus)
{
	int32 TotalDamage = 0;
	
	// Roll multiple dice (for striking runes, spell damage, etc.)
	for (int32 i = 0; i < DamageDieCount; i++)
	{
		TotalDamage += UKismetMathLibrary::RandomIntegerInRange(1, DamageDie);
	}
	
	// Add flat damage bonus
	return TotalDamage + DamageBonus;
}

int32 UPF2eCombatLibrary::RollD20(int32 MaxDieRoll)
{
	// Failsafe Clamp MaxDieRoll so If i accidentally make it 200 it wont break the game
	MaxDieRoll = FMath::Clamp(MaxDieRoll, 1, 20);
	
	// Roll 1 to MaxDieRoll (difficulty control)
	int32 Roll = UKismetMathLibrary::RandomIntegerInRange(1, MaxDieRoll);
	
	UE_LOG(LogGAS, VeryVerbose, TEXT("PF2eCombatLibrary: d20 roll = %d (MaxDie: %d)"), Roll, MaxDieRoll);
	
	return Roll;
}

EDegreeOfSuccess UPF2eCombatLibrary::RollAttackWithPenalty(int32 AttackBonus, int32 TargetAC, int32 RangePenalty, int32 MaxDieRoll)
{
	int32 D20Roll = RollD20(MaxDieRoll);
	int32 TotalRoll = D20Roll + AttackBonus - RangePenalty;
	
	UE_LOG(LogGAS, Verbose, TEXT("PF2eCombatLibrary: Attack roll %d + %d - %d = %d vs AC %d (MaxDie: %d)"), 
		D20Roll, AttackBonus, RangePenalty, TotalRoll, TargetAC, MaxDieRoll);
	
	return CalculateDegreeOfSuccess(TotalRoll, TargetAC, D20Roll);
}
