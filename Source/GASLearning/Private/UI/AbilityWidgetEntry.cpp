#include "UI/AbilityWidgetEntry.h"
#include "GAS/MyBaseGameplayAbility.h"
#include "Combatant.h"
#include "AbilitySystemComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
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
	
	// Create the widget UI first
	CreateEntryWidget();
	
	// Then update UI elements (icon, name, action cost, etc.)
	UpdateAbilityDisplay();
	
	UE_LOG(LogTemp, Log, TEXT("AbilityWidgetEntry: Setup complete for ability: %s"), 
		*AbilityReference->GetAbilityDisplayName().ToString());
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
		UE_LOG(LogTemp, Log, TEXT("AbilityWidgetEntry: Showing variants for %s"), 
			*AbilityReference->GetAbilityDisplayName().ToString());
	}
	else
	{
		// No variants - activate directly
		ActivateAbility();
	}
}

void UAbilityWidgetEntry::OnVariantSelected(EAbilityVariantType Variant)
{
	if (!AbilityReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityWidgetEntry: OnVariantSelected called with null ability reference"));
		return;
	}

	// Check if the selected variant is supported
	if (!AbilityReference->SupportedVariants.Contains(Variant))
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityWidgetEntry: Variant not supported by ability %s"), 
			*AbilityReference->GetAbilityDisplayName().ToString());
		return;
	}

	// Set the selected variant
	AbilityReference->SelectedVariant = Variant;
	
	// Hide variant selection UI
	HideVariantSelection();
	
	// Activate the ability with selected variant
	ActivateAbility();
	
	UE_LOG(LogTemp, Log, TEXT("AbilityWidgetEntry: Variant selected and ability activated"));
}

void UAbilityWidgetEntry::ActivateAbility()
{
	if (!AbilityReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityWidgetEntry: ActivateAbility called with null ability reference"));
		return;
	}

	// Check if we can afford the ability
	if (!CanAffordAbility())//This is failsafe. Activate ability shouldn't be pressable.
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityWidgetEntry: Cannot afford ability %s"), 
			*AbilityReference->GetAbilityDisplayName().ToString());
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

bool UAbilityWidgetEntry::CanAffordAbility() const
{
	if (!AbilityReference)
	{
		return false;
	}

	// TODO: Implement actual action cost checking
	// This should check if the current combatant has enough actions remaining
	// For now, just return true as a placeholder
	
	return true; // Placeholder
}