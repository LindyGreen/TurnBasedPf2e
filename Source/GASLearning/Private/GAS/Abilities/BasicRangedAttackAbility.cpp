#include "GAS/Abilities/BasicRangedAttackAbility.h"
#include "Combatant.h"
#include "GASLearning/Public/GAS/CombatAttributeSet.h"
#include "GAS/PF2eCombatLibrary.h"
#include "LogTypes.h"
#include "Kismet/KismetMathLibrary.h"

UBasicRangedAttackAbility::UBasicRangedAttackAbility()
{
	// Set ability UI data
	DisplayName = FText::FromString(TEXT("Ranged Attack"));
	Description = FText::FromString(TEXT("A basic ranged attack with a bow or crossbow"));
	ActionCost = 1;
	Range = 30; // Long range
	Category = EAbilityCategory::Attack;
	
	// Combat properties
	bRequiresTarget = true;
	bCanTargetSelf = false;
	bCanTargetAllies = false;
	bCanTargetEnemies = true;
	
	// Set default values
	MaxRange = 30; //Using up to 3 weapon increments due to map size limitations
	LongRangeThreshold = 10;//PF2e weapon increment
}

void UBasicRangedAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Call parent to handle action cost
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UE_LOG(LogGAS, Log, TEXT("BasicRangedAttackAbility: Executing ranged attack"));
	
	// For now, we'll simulate targeting - in a real implementation this would be handled by targeting system
	// TODO: Implement proper targeting system
	
	// Simulate range penalty calculation
	int32 RangePenalty = 0;
	// In real implementation: bool InRange = IsTargetInRange(Target, RangePenalty);
	
	// Simulate attack using PF2e degree of success system
	//First finds the target AC. Second finds the Edegree of success by rolling attack, then calculates the damage and effects based on the roll.
	//Will have to clean it up after all required functions are implemented to use children of this class for all ranged attacks
	int32 TargetAC = 15; // TODO: Get from actual target
	EDegreeOfSuccess AttackResult = RollAttack(RangePenalty, TargetAC);
	
	// Calculate damage once
	int32 Damage = RollDamage();
	
	switch (AttackResult)
	{
		case EDegreeOfSuccess::CriticalSuccess:
			{
				int32 DoubleDamage = Damage * 2;
				UE_LOG(LogGAS, Log, TEXT("BasicRangedAttackAbility: Critical Hit! Double damage: %d"), DoubleDamage);
				OnAttackCriticalSuccess(nullptr, DoubleDamage); // TODO: Pass actual target
			}
			break;
			
		case EDegreeOfSuccess::Success:
			UE_LOG(LogGAS, Log, TEXT("BasicRangedAttackAbility: Hit! Damage: %d"), Damage);
			OnAttackSuccess(nullptr, Damage); // TODO: Pass actual target
			break;
			
		case EDegreeOfSuccess::Failure:
			UE_LOG(LogGAS, Log, TEXT("BasicRangedAttackAbility: Miss!"));
			OnAttackFailure(nullptr); // TODO: Pass actual target
			break;
			
		case EDegreeOfSuccess::CriticalFailure:
			UE_LOG(LogGAS, Log, TEXT("BasicRangedAttackAbility: Critical Miss!"));
			OnAttackCriticalFailure(nullptr); // TODO: Pass actual target
			break;
	}
	
	// End the ability
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

EDegreeOfSuccess UBasicRangedAttackAbility::RollAttack(int32 RangePenalty, int32 TargetAC) const
{
	// Get attack bonus from GAS attributes
	ACombatant* Attacker = GetOwningCombatant();
	if (!Attacker || !Attacker->CombatAttributes)
	{
		UE_LOG(LogGAS, Error, TEXT("BasicRangedAttackAbility: No attacker or combat attributes found"));
		return EDegreeOfSuccess::CriticalFailure;
	}
	
	int32 AttackBonus = Attacker->CombatAttributes->GetAttackBonus();
	int32 MaxDieRoll = Attacker->CombatAttributes->GetMaxDieRoll();
	return UPF2eCombatLibrary::RollAttackWithPenalty(AttackBonus, TargetAC, RangePenalty, MaxDieRoll);//call to the library
}

int32 UBasicRangedAttackAbility::RollDamage() const
{
	// Get damage attributes from GAS
	ACombatant* Attacker = GetOwningCombatant();
	if (!Attacker || !Attacker->CombatAttributes)
	{
		UE_LOG(LogGAS, Error, TEXT("BasicRangedAttackAbility: No attacker or combat attributes found for damage"));
		// Fallback damage
		return UPF2eCombatLibrary::RollDamage(6, 1, 0);
	}
	
	// Use centralized damage rolling from PF2e Combat Library
	int32 DamageDie = Attacker->CombatAttributes->GetDamageDie();
	int32 DamageDieCount = Attacker->CombatAttributes->GetDamageDieCount();
	int32 DamageBonus = Attacker->CombatAttributes->GetDamageBonus();
	
	return UPF2eCombatLibrary::RollDamage(DamageDie, DamageDieCount, DamageBonus);
}

bool UBasicRangedAttackAbility::IsTargetInRange(ACombatant* Target, int32& OutRangePenalty) const
{
	OutRangePenalty = 0;
	
	if (!Target)
		return false;
		
	ACombatant* Attacker = GetOwningCombatant();
	if (!Attacker)
		return false;
	
	// Calculate distance
	FIntPoint AttackerPos = Attacker->LocationIndex;
	FIntPoint TargetPos = Target->LocationIndex;
	
	int32 Distance = FMath::Max(FMath::Abs(AttackerPos.X - TargetPos.X), FMath::Abs(AttackerPos.Y - TargetPos.Y));
	
	// Check if within maximum range
	if (Distance > MaxRange)
		return false;
	
	// Apply range penalty for long range shots
	if (Distance > LongRangeThreshold*2)
	{
		OutRangePenalty = 4; // -4 penalty for third weapon increment
	}
	if (Distance > LongRangeThreshold)
	{
		OutRangePenalty = 2; // -2 penalty for second increment
	}
	
	return true;
}

bool UBasicRangedAttackAbility::HasLineOfSight(ACombatant* Target) const
{
	// TODO: Implement line of sight checking with grid system
	// For now, assume we always have line of sight
	return true;
}