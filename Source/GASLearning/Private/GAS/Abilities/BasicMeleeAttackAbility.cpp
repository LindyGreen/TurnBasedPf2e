#include "GAS/Abilities/BasicMeleeAttackAbility.h"
#include "Combatant.h"
#include "GASLearning/Public/GAS/CombatAttributeSet.h"
#include "GAS/PF2eCombatLibrary.h"
#include "LogTypes.h"
#include "Kismet/KismetMathLibrary.h"

UBasicMeleeAttackAbility::UBasicMeleeAttackAbility()
{
	// Set ability UI data
	DisplayName = FText::FromString(TEXT("Melee Attack"));
	Description = FText::FromString(TEXT("A basic melee attack with a weapon"));
	ActionCost = 1;
	Range = 1; // Adjacent squares only
	Category = EAbilityCategory::Attack;
	
	// Combat properties
	bRequiresTarget = true;
	bCanTargetSelf = false;
	bCanTargetAllies = false;
	bCanTargetEnemies = true;

}

void UBasicMeleeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Call parent to handle action cost
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// For now, we'll simulate targeting - in a real implementation this would be handled by targeting system
	// TODO: Implement proper targeting system
	
	UE_LOG(LogGAS, Log, TEXT("BasicMeleeAttackAbility: Executing melee attack"));
	
	// Simulate attack against target with AC 15
	int32 TargetAC = 15; // TODO: Get from actual target
	EDegreeOfSuccess AttackResult = RollAttack(TargetAC);
	
	// Calculate damage once
	int32 Damage = RollDamage();
	
	switch (AttackResult)
	{
		case EDegreeOfSuccess::CriticalSuccess:
			{
				int32 DoubleDamage = Damage * 2;
				UE_LOG(LogGAS, Log, TEXT("BasicMeleeAttackAbility: Critical Hit! Double damage: %d"), DoubleDamage);
				OnAttackCriticalSuccess(nullptr, DoubleDamage); // TODO: Pass actual target
			}
			break;
			
		case EDegreeOfSuccess::Success:
			UE_LOG(LogGAS, Log, TEXT("BasicMeleeAttackAbility: Hit! Damage: %d"), Damage);
			OnAttackSuccess(nullptr, Damage); // TODO: Pass actual target
			break;
			
		case EDegreeOfSuccess::Failure:
			UE_LOG(LogGAS, Log, TEXT("BasicMeleeAttackAbility: Miss!"));
			OnAttackFailure(nullptr); // TODO: Pass actual target
			break;
			
		case EDegreeOfSuccess::CriticalFailure:
			UE_LOG(LogGAS, Log, TEXT("BasicMeleeAttackAbility: Critical Miss!"));
			OnAttackCriticalFailure(nullptr); // TODO: Pass actual target
			break;
	}
	
	// End the ability
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

EDegreeOfSuccess UBasicMeleeAttackAbility::RollAttack(int32 TargetAC) const
{
	// Get attack bonus from GAS attributes
	ACombatant* Attacker = GetOwningCombatant();
	if (!Attacker || !Attacker->CombatAttributes)
	{
		UE_LOG(LogGAS, Error, TEXT("BasicMeleeAttackAbility: No attacker or combat attributes found"));
		return EDegreeOfSuccess::CriticalFailure;
	}
	
	int32 AttackBonus = Attacker->CombatAttributes->GetAttackBonus();
	int32 MaxDieRoll = Attacker->CombatAttributes->GetMaxDieRoll();
	return UPF2eCombatLibrary::RollAttack(AttackBonus, TargetAC, MaxDieRoll);//call to the library
}

int32 UBasicMeleeAttackAbility::RollDamage() const
{
	// Get damage attributes from GAS
	ACombatant* Attacker = GetOwningCombatant();
	if (!Attacker || !Attacker->CombatAttributes)
	{
		UE_LOG(LogGAS, Error, TEXT("BasicMeleeAttackAbility: No attacker or combat attributes found for damage"));
		// Fallback damage, it's a crit fail so it doesn't matter whats inside.
		return UPF2eCombatLibrary::RollDamage(6, 1, 0);
	}
	
	// Use centralized damage rolling from PF2e Combat Library
	int32 DamageDie = Attacker->CombatAttributes->GetDamageDie();
	int32 DamageDieCount = Attacker->CombatAttributes->GetDamageDieCount();
	int32 DamageBonus = Attacker->CombatAttributes->GetDamageBonus();
	
	return UPF2eCombatLibrary::RollDamage(DamageDie, DamageDieCount, DamageBonus);
}

bool UBasicMeleeAttackAbility::IsTargetInRange(ACombatant* Target) const
{
	if (!Target)
		return false;
		
	ACombatant* Attacker = GetOwningCombatant();
	if (!Attacker)
		return false;
	
	// Check if target is adjacent (within 1 square)
	FIntPoint AttackerPos = Attacker->LocationIndex;
	FIntPoint TargetPos = Target->LocationIndex;
	
	int32 Distance = FMath::Max(FMath::Abs(AttackerPos.X - TargetPos.X), FMath::Abs(AttackerPos.Y - TargetPos.Y));
	
	return Distance <= Range;
}