#include "UI/InitiativeTrackerWidget.h"
#include "UI/InitiativeEntryWidget.h"
#include "TurnManagerComponent.h"
#include "Components/PanelWidget.h"
#include "Components/VerticalBox.h"
#include "LogTypes.h"
#include "UObject/ConstructorHelpers.h"

UInitiativeTrackerWidget::UInitiativeTrackerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Try to set default entry class to the Blueprint version
	// This path should match your WBP_InitiativeEntry location
	static ConstructorHelpers::FClassFinder<UInitiativeEntryWidget> EntryClassFinder(TEXT("/Game/PF2e/UI/WBP_InitiativeEntry"));
	if (EntryClassFinder.Succeeded())
	{
		InitiativeEntryClass = EntryClassFinder.Class;
	}
}

void UInitiativeTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UE_LOG(LogUI, Log, TEXT("InitiativeTrackerWidget: NativeConstruct called"));
}

void UInitiativeTrackerWidget::NativeDestruct()
{
	// Clean up delegate bindings safely
	if (TurnManagerRef && IsValid(TurnManagerRef))
	{
		TurnManagerRef->OnCombatStarted.RemoveDynamic(this, &UInitiativeTrackerWidget::OnCombatStarted);
		TurnManagerRef->OnTurnChanged.RemoveDynamic(this, &UInitiativeTrackerWidget::OnTurnChanged);
		TurnManagerRef->OnCombatantActionsChanged.RemoveDynamic(this, &UInitiativeTrackerWidget::UpdateCombatantActions);
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
	TurnManagerRef->OnCombatantActionsChanged.AddDynamic(this, &UInitiativeTrackerWidget::UpdateCombatantActions);
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

void UInitiativeTrackerWidget::UpdateCombatantActions(ACombatant* Combatant, int32 ActionsRemaining)
{
	UInitiativeEntryWidget* EntryWidget = FindEntryForCombatant(Combatant);
	if (EntryWidget)
	{
		EntryWidget->UpdateActionsDisplay(ActionsRemaining, Combatant->GetMaxActions());
	}
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