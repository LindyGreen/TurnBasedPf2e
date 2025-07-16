#include "GAS/MyBaseGameplayAbility.h"
#include "Combatant.h"
#include "LogTypes.h"
#include "AbilitySystemComponent.h"

#include "GAS/CombatAttributeSet.h"

UMyBaseGameplayAbility::UMyBaseGameplayAbility()
{
	// Set default values
	DisplayName = FText::FromString(TEXT("Base Ability"));
	Description = FText::FromString(TEXT("A basic ability"));
	ActionCost = 1;
	Range = 5;
	bRequiresTarget = true;
	bCanTargetSelf = false;
	bCanTargetAllies = false;
	bCanTargetEnemies = true;
	Category = EAbilityCategory::Attack;
	
	// Default ability settings
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UMyBaseGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Check if we can afford the ability
	if (!TrySpendActions(ActionCost))
	{
		UE_LOG(LogGAS, Warning, TEXT("MyBaseGameplayAbility: Cannot afford ability %s - insufficient actions"), *DisplayName.ToString());
		OnAbilityActivationFailed();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UE_LOG(LogGAS, Log, TEXT("MyBaseGameplayAbility: Activated ability %s"), *DisplayName.ToString());
	
	// Call the Blueprint event for successful activation
	OnAbilityActivationSuccess();
	
	// Call parent implementation
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMyBaseGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogGAS, Log, TEXT("MyBaseGameplayAbility: Ended ability %s (Cancelled: %s)"), 
		*DisplayName.ToString(), bWasCancelled ? TEXT("Yes") : TEXT("No"));
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UMyBaseGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// First check parent conditions
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// Check if we have enough actions
	ACombatant* Combatant = GetCombatantFromActorInfo(ActorInfo);
	if (!Combatant)
	{
		UE_LOG(LogGAS, Warning, TEXT("MyBaseGameplayAbility: Cannot get combatant from ActorInfo"));
		return false;
	}

	if (Combatant->CombatAttributes->GetActionsRemaining() < ActionCost)
	{
		UE_LOG(LogGAS, Verbose, TEXT("MyBaseGameplayAbility: Insufficient actions for %s (Required: %d, Available: %f)"), 
			*DisplayName.ToString(), ActionCost, Combatant->CombatAttributes->GetActionsRemaining());
		return false;
	}

	return true;
}

bool UMyBaseGameplayAbility::TrySpendActions(int32 ActionsToSpend)
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (!ActorInfo)
	{
	//	UE_LOG(LogGAS, Warning, TEXT("MyBaseGameplayAbility: No ActorInfo available"));
		return false;
	}

	ACombatant* Combatant = GetCombatantFromActorInfo(ActorInfo);
	if (!Combatant)
	{
	//	UE_LOG(LogGAS, Warning, TEXT("MyBaseGameplayAbility: Cannot get combatant"));
		return false;
	}

	// Check if we have enough actions
	if (Combatant->CombatAttributes->GetActionsRemaining() < ActionsToSpend)
	{
//		UE_LOG(LogGAS, Warning, TEXT("MyBaseGameplayAbility: Insufficient actions (Required: %d, Available: %f)"), ActionsToSpend, Combatant->CombatAttributes->GetActionsRemaining());
		return false;
	}

	// Spend the actions
	//Combatant->SpendAction(ActionsToSpend);
	//UE_LOG(LogGAS, Log, TEXT("MyBaseGameplayAbility: Spent %d actions"), ActionsToSpend);
	
	return true;
}

ACombatant* UMyBaseGameplayAbility::GetOwningCombatant() const
{
	return GetCombatantFromActorInfo(GetCurrentActorInfo());
}

ACombatant* UMyBaseGameplayAbility::GetCombatantFromActorInfo(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	return Cast<ACombatant>(ActorInfo->AvatarActor.Get());
}