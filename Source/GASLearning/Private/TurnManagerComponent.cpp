#include "TurnManagerComponent.h"
#include "Grid.h"
#include "Combatant.h"

UTurnManagerComponent::UTurnManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

AGrid* UTurnManagerComponent::GetOwningGrid() const
{
	return Cast<AGrid>(GetOwner());
}

void UTurnManagerComponent::StartCombat()
{
	// Check if combatant array is not empty
	if (CombatantArray.IsEmpty()) return;
	
	// Create temporary initiative holder map
	TMap<float, TObjectPtr<ACombatant>> TempInitiativeHolder;
	
	// Loop through each combatant
	for (auto& Combatant : CombatantArray)//auto is just auto type finder for QOL 
	{
		// Roll initiative for this combatant
		float Initiative = Combatant->RollInitiative();
		
		// TODO: GetIndex and SetUnitIndexOnGrid
		
		// Add to temp map
		TempInitiativeHolder.Add(Initiative, Combatant);
	}
	
	// Clear the combatant array
	CombatantArray.Empty();
	
	// Get all keys from temp map
	TArray<float> KeysToSort;
	TempInitiativeHolder.GetKeys(KeysToSort);
	
	// Sort keys in descending order
	KeysToSort.Sort([](const float& A, const float& B) { return A > B; });
	
	// Rebuild combatant array in sorted order
	for (const float& Key : KeysToSort)
	{
		if (TObjectPtr<ACombatant>* FoundCombatant = TempInitiativeHolder.Find(Key))
		{
			CombatantArray.Add(*FoundCombatant);
		}
	}
}

void UTurnManagerComponent::SpendActions(int32 ActionsToSpend)
{
	CurrentTurnActions=CurrentTurnActions-ActionsToSpend;
	if (CurrentTurnActions <= 0)
		{EndTurn();}
}

void UTurnManagerComponent::EndTurn()
{
	if (CurrentCombatant){CurrentCombatant->EndTurnEffects();} //Applies end of turn effects on the combatant
	CombatantTurn++; if (CombatantTurn >= CombatantArray.Num()){CombatantTurn=0;} //returns the initiative to the start
	if(CombatantArray[CombatantTurn])
	{
		CurrentCombatant=CombatantArray[CombatantTurn];//Sets the combatant who's turn should be as active combatant
		CurrentCombatant->BeginTurn(); //Starts new combatants turn
	}
	//TODO Call On Turn Changed to WBP initiative tracker should be here.
}
