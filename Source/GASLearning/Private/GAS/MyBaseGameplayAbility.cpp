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
	// Cancel all other active abilities first (unless this is a sub-ability)
	if (!bIsSubAbility)
	{
		CancelAllOtherActiveAbilities();
	}

	// Let GAS handle cost checking and spending via Cost GameplayEffect

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

void UMyBaseGameplayAbility::CancelAllOtherActiveAbilities()
{
	ACombatant* Combatant = GetOwningCombatant();
	if (!Combatant)
	{
		return;
	}

	UAbilitySystemComponent* ASC = Combatant->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	FGameplayAbilitySpecHandle CurrentHandle = GetCurrentAbilitySpecHandle();
	TArray<FGameplayAbilitySpecHandle> AbilitiesToCancel;
	
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.IsActive() && Spec.Handle != CurrentHandle)
		{
			AbilitiesToCancel.Add(Spec.Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& HandleToCancel : AbilitiesToCancel)
	{
		ASC->CancelAbilityHandle(HandleToCancel);
	}
}