#include "UI/InitiativeEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"
#include "../GAS/CombatAttributeSet.h"
#include "LogTypes.h"

UInitiativeEntryWidget::UInitiativeEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UInitiativeEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInitiativeEntryWidget::SetupEntry(ACombatant* InCombatant, float InitiativeValue)
{
	if (!InCombatant)
	{
		UE_LOG(LogUI, Warning, TEXT("InitiativeEntryWidget: SetupEntry called with null combatant"));
		return;
	}

	CombatantRef = InCombatant;

	// Set character name
	if (CharacterNameText)
	{
		CharacterNameText->SetText(InCombatant->CharacterName);
		UE_LOG(LogUI, Log, TEXT("InitiativeEntryWidget: Set character name to: %s"), *InCombatant->CharacterName.ToString());
	}
	else
	{
		UE_LOG(LogUI, Warning, TEXT("InitiativeEntryWidget: CharacterNameText is null - check Blueprint binding"));
	}

	// Set initiative value
	if (InitiativeValueText)
	{
		FText InitText = FText::AsNumber(FMath::RoundToInt(InitiativeValue));
		InitiativeValueText->SetText(InitText);
		UE_LOG(LogUI, Log, TEXT("InitiativeEntryWidget: Set initiative to: %s"), *InitText.ToString());
	}
	else
	{
		UE_LOG(LogUI, Warning, TEXT("InitiativeEntryWidget: InitiativeValueText is null - check Blueprint binding"));
	}

	// Bind to attribute change delegates for real-time updates
	if (InCombatant->CombatAttributes)
	{
		InCombatant->CombatAttributes->OnHealthChanged.AddDynamic(this, &UInitiativeEntryWidget::OnHealthChanged);
		InCombatant->CombatAttributes->OnActionsRemainingChanged.AddDynamic(this, &UInitiativeEntryWidget::OnActionsChanged);
	}

	// Update all displays
	UpdateDisplay();

	UE_LOG(LogUI, Log, TEXT("InitiativeEntryWidget: Setup entry for %s with initiative %f"), 
		*InCombatant->CharacterName.ToString(), InitiativeValue);
}

void UInitiativeEntryWidget::UpdateActionsDisplay(int32 ActionsRemaining, int32 MaxActions)
{
	if (ActionsRemainingText)
	{
		FText ActionText = FText::FromString(FString::Printf(TEXT("%d/%d"), ActionsRemaining, MaxActions));
		ActionsRemainingText->SetText(ActionText);
	}
}

void UInitiativeEntryWidget::UpdateHealthDisplay()
{
	if (!CombatantRef || !IsValid(CombatantRef) || !HealthBar)
		return;

	float CurrentHealth = CombatantRef->GetCurrentHealth();
	float MaxHealth = CombatantRef->GetMaxHealth();
	
	if (MaxHealth > 0)
	{
		float HealthPercent = CurrentHealth / MaxHealth;
		HealthBar->SetPercent(HealthPercent);
		
		// Change color based on health percentage
		FLinearColor HealthColor = FLinearColor::Green;
		if (HealthPercent < 0.3f)
		{
			HealthColor = FLinearColor::Red;
		}
		else if (HealthPercent < 0.6f)
		{
			HealthColor = FLinearColor::Yellow;
		}
		
		HealthBar->SetFillColorAndOpacity(HealthColor);
	}
}

void UInitiativeEntryWidget::SetCurrentTurn(bool bIsCurrentTurn)
{
	if (EntryBorder)
	{
		FLinearColor BorderColor = bIsCurrentTurn ? CurrentTurnColor : NormalColor;
		EntryBorder->SetBrushColor(BorderColor);
		UE_LOG(LogUI, Log, TEXT("InitiativeEntryWidget: Set current turn %s for %s"), 
			bIsCurrentTurn ? TEXT("TRUE") : TEXT("FALSE"),
			CombatantRef ? *CombatantRef->CharacterName.ToString() : TEXT("Unknown"));
	}
	else
	{
		UE_LOG(LogUI, Warning, TEXT("InitiativeEntryWidget: EntryBorder is null - check Blueprint binding"));
	}
	
	// Show/hide actions display based on current turn
	if (ActionsRemainingText)
	{
		ESlateVisibility Visibility = bIsCurrentTurn ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		ActionsRemainingText->SetVisibility(Visibility);
	}
}

void UInitiativeEntryWidget::UpdateReactionDisplay(bool bHasReaction)
{
	// This can be extended to show reaction status
	// For now, we'll include it in the actions display
}

void UInitiativeEntryWidget::UpdateDisplay()
{
	if (!CombatantRef || !IsValid(CombatantRef))
		return;

	// Update actions
	UpdateActionsDisplay(CombatantRef->GetActionsRemaining(), CombatantRef->GetMaxActions());
	
	// Update health
	UpdateHealthDisplay();
	
	// Update reaction status
	UpdateReactionDisplay(CombatantRef->GetReactionAvailable());
}

void UInitiativeEntryWidget::OnHealthChanged(float Magnitude, float NewHealth)
{
	UE_LOG(LogUI, Log, TEXT("InitiativeEntryWidget: Health changed for %s - New Health: %f"), 
		CombatantRef ? *CombatantRef->CharacterName.ToString() : TEXT("Unknown"), NewHealth);
	UpdateHealthDisplay();
}

void UInitiativeEntryWidget::OnActionsChanged(float Magnitude, float NewActionsRemaining)
{
	UE_LOG(LogUI, Log, TEXT("InitiativeEntryWidget: Actions changed for %s - Remaining: %f"), 
		CombatantRef ? *CombatantRef->CharacterName.ToString() : TEXT("Unknown"), NewActionsRemaining);
	
	if (CombatantRef)
	{
		UpdateActionsDisplay(NewActionsRemaining, CombatantRef->GetMaxActions());
	}
}