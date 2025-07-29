#include "UI/InitiativeTrackerWidget.h"
#include "UI/InitiativeEntryWidget.h"
#include "UI/AbilityWidgetEntry.h"
#include "TurnManagerComponent.h"
#include "Components/PanelWidget.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "LogTypes.h"

#include "GAS/CombatAttributeSet.h"
#include "GAS/MyBaseGameplayAbility.h"
#include "AbilitySystemComponent.h"


UInitiativeTrackerWidget::UInitiativeTrackerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UInitiativeTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Set text for button labels
	if (TextEndTurn)
	{
		TextEndTurn->SetText(FText::FromString(TEXT("End Turn")));
	}
	
	if (TextRollInitiative)
	{
		TextRollInitiative->SetText(FText::FromString(TEXT("Roll Initiative")));
	}
	
	UE_LOG(LogUI, Log, TEXT("InitiativeTrackerWidget: NativeConstruct called"));
}

void UInitiativeTrackerWidget::NativeDestruct()
{
	// Clean up delegate bindings safely
	if (TurnManagerRef && IsValid(TurnManagerRef))
	{
		TurnManagerRef->OnCombatStarted.RemoveDynamic(this, &UInitiativeTrackerWidget::OnCombatStarted);
		TurnManagerRef->OnTurnChanged.RemoveDynamic(this, &UInitiativeTrackerWidget::OnTurnChanged);
		TurnManagerRef->OnCombatEnded.RemoveDynamic(this, &UInitiativeTrackerWidget::OnCombatEnded);
		TurnManagerRef = nullptr;
	}
	
	// Clear all entry widgets
	EntryWidgets.Empty();
	
	Super::NativeDestruct();
}

void UInitiativeTrackerWidget::InitializeTracker(UTurnManagerComponent* InTurnManager)
{
	if (!InTurnManager)
	{
		UE_LOG(LogUI, Warning, TEXT("InitiativeTrackerWidget: Attempted to initialize with null TurnManager"));
		return;
	}

	TurnManagerRef = InTurnManager;
	
	// Bind to delegates
	TurnManagerRef->OnCombatStarted.AddDynamic(this, &UInitiativeTrackerWidget::OnCombatStarted);
	TurnManagerRef->OnTurnChanged.AddDynamic(this, &UInitiativeTrackerWidget::OnTurnChanged);
	TurnManagerRef->OnCombatEnded.AddDynamic(this, &UInitiativeTrackerWidget::OnCombatEnded);
	
	UE_LOG(LogUI, Log, TEXT("InitiativeTrackerWidget: Initialized with TurnManager"));
}

void UInitiativeTrackerWidget::UpdateInitiativeOrder(const TArray<ACombatant*>& SortedCombatants)
{
	if (!InitiativeList)
	{
		UE_LOG(LogUI, Warning, TEXT("InitiativeTrackerWidget: InitiativeList panel is null"));
		return;
	}

	ClearTracker();

	// Create entry widgets for each combatant
	for (int32 i = 0; i < SortedCombatants.Num(); ++i)
	{
		if (SortedCombatants[i])
		{
			CreateEntryWidget(SortedCombatants[i], i);
		}
	}

	UE_LOG(LogUI, Log, TEXT("InitiativeTrackerWidget: Updated initiative order with %d combatants"), SortedCombatants.Num());
}

void UInitiativeTrackerWidget::UpdateCurrentTurn(ACombatant* CurrentCombatant)
{
	// Update all entries to show which one is current
	for (UInitiativeEntryWidget* EntryWidget : EntryWidgets)
	{
		if (EntryWidget && EntryWidget->CombatantRef)
		{
			bool bIsCurrentTurn = (EntryWidget->CombatantRef == CurrentCombatant);
			EntryWidget->SetCurrentTurn(bIsCurrentTurn);
		}
	}

	UE_LOG(LogUI, Log, TEXT("InitiativeTrackerWidget: Updated current turn to %s"), 
		CurrentCombatant ? *CurrentCombatant->CharacterName.ToString() : TEXT("None"));
}

void UInitiativeTrackerWidget::ClearTracker()
{
	if (InitiativeList)
	{
		InitiativeList->ClearChildren();
	}
	EntryWidgets.Empty();
	
	UE_LOG(LogUI, Log, TEXT("InitiativeTrackerWidget: Cleared tracker"));
}

void UInitiativeTrackerWidget::OnCombatStarted(const TArray<ACombatant*>& SortedCombatants)
{
	UpdateInitiativeOrder(SortedCombatants);
}

void UInitiativeTrackerWidget::OnTurnChanged(ACombatant* NewCurrentCombatant)
{
	UpdateCurrentTurn(NewCurrentCombatant);
	
	CreateAbilityWidgetEntry(NewCurrentCombatant);
}

void UInitiativeTrackerWidget::OnCombatEnded()
{
	// Clear all initiative entries when combat ends
	ClearTracker();
	UE_LOG(LogUI, Log, TEXT("InitiativeTrackerWidget: Combat ended, tracker cleared"));
}

void UInitiativeTrackerWidget::CreateEntryWidget(ACombatant* Combatant, int32 Index)
{
	if (!Combatant || !InitiativeEntryClass)
	{
		UE_LOG(LogUI, Warning, TEXT("InitiativeTrackerWidget: Cannot create entry - Combatant or EntryClass is null"));
		return;
	}

	// Create the entry widget
	UInitiativeEntryWidget* EntryWidget = CreateWidget<UInitiativeEntryWidget>(this, InitiativeEntryClass);
	if (!EntryWidget)
	{
		UE_LOG(LogUI, Error, TEXT("InitiativeTrackerWidget: Failed to create InitiativeEntryWidget"));
		return;
	}

	// Get initiative value from TurnManager's map
	float InitiativeValue = 0.0f;
	if (TurnManagerRef)
	{
		// Find the initiative value for this combatant
		for (const auto& Pair : TurnManagerRef->CurrentInitiativeHolder)
		{
			if (Pair.Value == Combatant)
			{
				InitiativeValue = Pair.Key;
				break;
			}
		}
	}

	// Setup the entry
	EntryWidget->SetupEntry(Combatant, InitiativeValue);
	
	// Bind to the ability refresh delegate
	//Note, I know it creates a bunch of useless calls, when enemy AI will move,
	// but that is a sacrifice I am willing to make
	EntryWidget->OnActionsChangedForAbilityRefresh.AddDynamic(this, &UInitiativeTrackerWidget::RefreshAllAbilityButtons);
	
	// Add to the list
	InitiativeList->AddChild(EntryWidget);
	EntryWidgets.Add(EntryWidget);
	
	UE_LOG(LogUI, Log, TEXT("InitiativeTrackerWidget: Created entry for %s with initiative %f"), 
		*Combatant->CharacterName.ToString(), InitiativeValue);
}

UInitiativeEntryWidget* UInitiativeTrackerWidget::FindEntryForCombatant(ACombatant* Combatant)
{
	for (UInitiativeEntryWidget* EntryWidget : EntryWidgets)
	{
		if (EntryWidget && EntryWidget->CombatantRef == Combatant)
		{
			return EntryWidget;
		}
	}
	return nullptr;
}

void UInitiativeTrackerWidget::RefreshAllAbilityButtons()
{
	if (!AbilityHotbar || AbilityHotbar->GetChildrenCount() == 0) // Quit if AI
	{ 
		// TODO make a clear separation to not call this when it's AI's turn 
		UE_LOG(LogUI, Warning, TEXT("InitiativeTrackerWidget: AbilityHotbar is null or no Abilities on the bar (AI)"));
		return;
	}

	// Loop through all current ability widgets and call EnableDisableAbilityButton
	for (int32 i = 0; i < AbilityHotbar->GetChildrenCount(); ++i)
	{
		UAbilityWidgetEntry* AbilityWidget = Cast<UAbilityWidgetEntry>(AbilityHotbar->GetChildAt(i));
		if (AbilityWidget)
		{
			AbilityWidget->EnableDisableAbilityButton();
		}
	}
}

void UInitiativeTrackerWidget::CreateAbilityWidgetEntry(ACombatant* Combatant)
{


	if (!AbilityWidgetClass)
	{
		UE_LOG(LogUI, Warning, TEXT("AbilityWidgetClass is null - check WBP_InitiativeTracker UI section"));
		return;
	}

	// Clear existing ability widgets
	AbilityHotbar->ClearChildren();

	// Get the combatant's Ability System Component
	UAbilitySystemComponent* ASC = Combatant->GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogUI, Warning, TEXT("InitiativeTrackerWidget: Combatant %s has no AbilitySystemComponent"), 
			*Combatant->CharacterName.ToString());
		return;
	}

	// Get all abilities from the ASC
	TArray<FGameplayAbilitySpec> AbilitySpecs = ASC->GetActivatableAbilities();
	

	// Create widgets for each ability
	for (const FGameplayAbilitySpec& AbilitySpec : AbilitySpecs)
	{
		if (!AbilitySpec.Ability)continue; //SafetyCheck, 

		// Cast to our base ability class to get UI data
		UMyBaseGameplayAbility* BaseAbility = Cast<UMyBaseGameplayAbility>(AbilitySpec.Ability);
		if (!BaseAbility) 
		{
			UE_LOG(LogUI, Log, TEXT("Ability you put in your spawning is not a child of MyBaseGameplayAbiltiy"));
		continue; //SafetyTypeCheck only children of BaseGameplayAbiltiyAllowed
		}
		

		// Create the ability widget with ASC parameter
		UAbilityWidgetEntry* AbilityWidget = CreateWidget<UAbilityWidgetEntry>(this, AbilityWidgetClass);
		AbilityWidget->OwnerASC = ASC;

		// Setup the widget with the ability
		AbilityWidget->SetupAbilityWidget(BaseAbility);
		// Add to the hotbar
		AbilityHotbar->AddChild(AbilityWidget);
		
	}
	
	// Add EndTurn button after all abilities
	if (EndTurnButton)
	{
		AbilityHotbar->AddChild(EndTurnButton);
	}
}

