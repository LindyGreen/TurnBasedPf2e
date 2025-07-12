#include "GAS/Abilities/BasicDamageSpellAbility.h"
#include "Combatant.h"
#include "GAS/PF2eCombatLibrary.h"
#include "LogTypes.h"
#include "Kismet/KismetMathLibrary.h"

UBasicDamageSpellAbility::UBasicDamageSpellAbility()
{
	// Set ability UI data
	DisplayName = FText::FromString(TEXT("Magic Missile"));
	Description = FText::FromString(TEXT("A basic damage spell that fires magical projectiles"));
	ActionCost = 2; // Most spells cost 2 actions 
	// in PF2e this one is 1-3 actions so this will be a bit more differently made later 
	Range = 24; // 120 feet converted to squares
	Category = EAbilityCategory::Spell;
	
	// Combat properties
	bRequiresTarget = true;
	bCanTargetSelf = false;
	bCanTargetAllies = false;
	bCanTargetEnemies = true;
	
	// Spell specific properties
	SpellLevel = 1;
	AreaType = ESpellArea::Single;
	AreaSize = 1;
	BaseDamage = 4;
	DamageDiceCount = 2;
	bAllowsSave = true;//this spell juist deals damage, only roll is the damage roll
	SaveDC = 15;
}

void UBasicDamageSpellAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Call parent to handle action cost
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UE_LOG(LogGAS, Log, TEXT("BasicDamageSpellAbility: Casting %s"), *DisplayName.ToString());
	
	// For now, we'll simulate targeting and area calculation
	// TODO: Implement proper targeting system and area calculation
	
	// Calculate damage
	int32 Damage = RollDamage();
	UE_LOG(LogGAS, Log, TEXT("BasicDamageSpellAbility: Base damage rolled: %d"), Damage);
	
	// Handle saving throw if applicable
	if (bAllowsSave)
	{
		EDegreeOfSuccess SaveResult = RollSavingThrow();
		
		switch (SaveResult)
		{
			case EDegreeOfSuccess::CriticalSuccess:
				UE_LOG(LogGAS, Log, TEXT("BasicDamageSpellAbility: Critical Success! No damage"));
				OnSpellCriticalSuccess(nullptr); // TODO: Pass actual target
				break;
				
			case EDegreeOfSuccess::Success:
				{
					int32 HalfDamage = Damage / 2;
					UE_LOG(LogGAS, Log, TEXT("BasicDamageSpellAbility: Success! Half damage: %d"), HalfDamage);
					OnSpellSuccess(nullptr, HalfDamage); // TODO: Pass actual target
				}
				break;
				
			case EDegreeOfSuccess::Failure:
				UE_LOG(LogGAS, Log, TEXT("BasicDamageSpellAbility: Failure! Full damage: %d"), Damage);
				OnSpellFailure(nullptr, Damage); // TODO: Pass actual target
				break;
				
			case EDegreeOfSuccess::CriticalFailure:
				{
					int32 DoubleDamage = Damage * 2;
					UE_LOG(LogGAS, Log, TEXT("BasicDamageSpellAbility: Critical Failure! Double damage: %d"), DoubleDamage);
					OnSpellCriticalFailure(nullptr, DoubleDamage); // TODO: Pass actual target
				}
				break;
		}
	}
	else
	{
		// No save allowed, spell automatically deals full damage
		UE_LOG(LogGAS, Log, TEXT("BasicDamageSpellAbility: No save allowed! Full damage: %d"), Damage);
		OnSpellFailure(nullptr, Damage); // TODO: Pass actual target
	}
	
	// End the ability
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

int32 UBasicDamageSpellAbility::RollDamage() const
{
	// Use centralized damage rolling from PF2e Combat Library
	// For spells, use the spell's own damage dice instead of weapon dice
	return UPF2eCombatLibrary::RollDamage(BaseDamage, DamageDiceCount, 0);
}

EDegreeOfSuccess UBasicDamageSpellAbility::RollSavingThrow(int32 TargetSaveBonus) const
{
	// Use the centralized PF2e combat library
	return UPF2eCombatLibrary::RollSave(TargetSaveBonus, SaveDC);
}

TArray<FIntPoint> UBasicDamageSpellAbility::GetSpellArea(FIntPoint TargetLocation) const
{
	// Get the caster for reference
	ACombatant* Caster = GetOwningCombatant();
	if (!Caster)
	{
		TArray<FIntPoint> SingleTarget;
		SingleTarget.Add(TargetLocation);
		return SingleTarget;
	}

	// Get RangeFinder from the grid
	// TODO: This should be cached or passed in properly
	// For now, we'll return a simple single target
	TArray<FIntPoint> AffectedSquares;
	
	switch (AreaType)
	{
		case ESpellArea::Single:
			AffectedSquares.Add(TargetLocation);
			break;
			
		case ESpellArea::Burst:
			// Use RangeFinder's GenerateBurst when we have access to it
			// For now, simple implementation
			AffectedSquares.Add(TargetLocation);
			break;
			
		case ESpellArea::Emanation:
			// Use RangeFinder's GenerateEmanation when we have access to it
			// For now, simple implementation
			AffectedSquares.Add(TargetLocation);
			break;
			
		case ESpellArea::Line:
			// Use RangeFinder's GenerateLine when we have access to it
			AffectedSquares.Add(TargetLocation);
			break;
			
		case ESpellArea::Cone:
			// Use RangeFinder's GenerateCone when we have access to it
			AffectedSquares.Add(TargetLocation);
			break;
	}
	
	return AffectedSquares;
}