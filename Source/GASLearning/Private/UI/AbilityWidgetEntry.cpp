#include "UI/AbilityWidgetEntry.h"
#include "GAS/MyBaseGameplayAbility.h"
#include "Combatant.h"
#include "AbilitySystemComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "GameplayAbilitySpec.h"

#include "Components/TextBlock.h"
#include "LogTypes.h"

void UAbilityWidgetEntry::SetupAbilityWidget(UMyBaseGameplayAbility* InAbility)
{
	if (!InAbility)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityWidgetEntry: Trying to setup with null ability"));
		return;
	}

	AbilityReference = InAbility;

	// Create the widget UI first. This will handle the widget variation logic
	CreateEntryWidget();
	// Then update UI elements (icon, name, action cost, etc.)
	SetupAbilityDisplay();

	// Cache ability spec and do initial button state check
	if (OwnerASC)
	{
		CachedAbilitySpec = OwnerASC->FindAbilitySpecFromClass(AbilityReference->GetClass());

		// Initial button state check
		EnableDisableAbilityButton();
	}
}

void UAbilityWidgetEntry::OnAbilityClicked()
{
	if (!AbilityReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityWidgetEntry: OnAbilityClicked called with null ability reference"));
		return;
	}

	// Check if ability has variants
	if (AbilityReference->bHasVariants && AbilityReference->SupportedVariants.Num() > 0)
	{
		// Show variant selection UI
		ShowVariantSelection();
		UE_LOG(LogTemp, Log, TEXT("AbilityWidgetEntry: Showing variants for %s"),*AbilityReference->GetAbilityDisplayName().ToString());
	}
	else
	{
		// No variants - activate directly
		ActivateAbility();
	}
}

void UAbilityWidgetEntry::OnVariantSelected(EAbilityVariantType Variant)
{
//TODO Figure out variant logic
	HideVariantSelection();
	ActivateAbility();
}

void UAbilityWidgetEntry::SetupAbilityDisplay()
{
	AbilityIcon->SetBrushFromTexture(AbilityReference->GetAbilityIcon());
	//set icon
	AbilityName->SetText(AbilityReference->GetAbilityDisplayName());
	SetToolTipText(AbilityReference->GetAbilityDescription());
	ActionCost->SetBrushFromTexture(AbilityReference->GetAbilityIcon());
	//TODO This has to be action cost icon,
	//TODO --- so put action Icons into here and Select based on Action cost
}

void UAbilityWidgetEntry::ActivateAbility()
{
	if (!AbilityReference)
	{
		UE_LOG(LogTemp, Warning,TEXT("AbilityWidgetEntry: ActivateAbility called with null ability reference"));
		return;
	}

	// Use the ASC reference passed when spawning the widget
	if (!OwnerASC)
	{
		UE_LOG(LogTemp, Error, TEXT("AbilityWidgetEntry: OwnerASC is null - widget not properly initialized"));
		return;
	}

	// Try to activate the ability
	bool bSuccess = OwnerASC->TryActivateAbilityByClass(AbilityReference->GetClass());
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("AbilityWidgetEntry: Successfully activated ability %s"), 
			*AbilityReference->GetAbilityDisplayName().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityWidgetEntry: Failed to activate ability %s"), 
			*AbilityReference->GetAbilityDisplayName().ToString());
	}
}

void UAbilityWidgetEntry::EnableDisableAbilityButton()
{
	UE_LOG(LogGAS, Log,TEXT("EnableDisableAbilityButton called"));


	// Use GAS built-in ability activation checking
	FGameplayTagContainer FailureTags;
	
	// Debug: Check what cost GE the ability has
	UMyBaseGameplayAbility* BaseAbility = Cast<UMyBaseGameplayAbility>(CachedAbilitySpec->Ability);
	if (BaseAbility)
	{
		UE_LOG(LogGAS, Log, TEXT("AbilityWidgetEntry: Checking ability %s"), *BaseAbility->GetAbilityDisplayName().ToString());
		UGameplayEffect* CostGE = BaseAbility->GetCostGameplayEffect();
		UE_LOG(LogGAS, Log, TEXT("AbilityWidgetEntry: Ability has cost GE: %s"), CostGE ? *CostGE->GetName() : TEXT("None"));
	}
	
	// First check the cost separately
	bool bCanAffordCost = CachedAbilitySpec->Ability->CheckCost(CachedAbilitySpec->Handle, OwnerASC->AbilityActorInfo.Get());
	UE_LOG(LogGAS, Log, TEXT("AbilityWidgetEntry: CheckCost result %s"), bCanAffordCost ? TEXT("true") : TEXT("false"));
	
	// Then check full activation
	bool bCanAfford = CachedAbilitySpec->Ability->CanActivateAbility(CachedAbilitySpec->Handle, OwnerASC->AbilityActorInfo.Get(), nullptr, nullptr, &FailureTags);
	UE_LOG(LogGAS, Log, TEXT("AbilityWidgetEntry: CanActivateAbility result %s"), bCanAfford ? TEXT("true") : TEXT("false"));
	
	if (!FailureTags.IsEmpty())
	{
		UE_LOG(LogGAS, Log, TEXT("AbilityWidgetEntry: Failure tags: %s"), *FailureTags.ToString());
	}
	
	AbilityButton->SetIsEnabled(bCanAfford);
}