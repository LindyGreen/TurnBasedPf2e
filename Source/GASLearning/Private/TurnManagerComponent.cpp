#include "TurnManagerComponent.h"
#include "Grid.h"
#include "Combatant.h"
#include "LogTypes.h"
UTurnManagerComponent::UTurnManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTurnManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get Grid reference from owner
	if (AActor* Owner = GetOwner()) GridReference = Cast<AGrid>(Owner);
}


void UTurnManagerComponent::InitializeCombat()
{
	if (GridReference)
	{
		SpawnEnemies(GridReference->EnemiesOnMapRowName); //This is currently implemented in blueprint
	}
}
void UTurnManagerComponent::SetUnitOnGrid(ACombatant* Combatant, FIntPoint Index, bool Force)
{
	if (!Combatant || !GridReference) return;
	
	// Check if we need to move the unit
	if (Combatant->LocationIndex != Index || Force)
	{
		// Clear unit from current tile
		if (FS_TileData* CurrentTile = GridReference->GridTiles.Find(Combatant->LocationIndex))
		{
			CurrentTile->UnitOnTile = nullptr;
		}
		
		// Set unit on new tile
		if (FS_TileData* NewTile = GridReference->GridTiles.Find(Index))
		{
			NewTile->UnitOnTile = Combatant;
			Combatant->LocationIndex = Index;
		}
	}
}
void UTurnManagerComponent::StartCombat() //StartInitiativeButtonActivates this.
{
	// Check if combatant's array is not empty
	if (CombatantArray.IsEmpty()) 
	{
		UE_LOG(Log_TurnManager, Error, TEXT("CombatantArray is empty! Cannot start combat."));
		return;
	}
	//Clears Selected state from tiles
	GridReference->ClearStateFromTiles(ETileState::Selected);
	UE_LOG(Log_TurnManager, Display, TEXT("Starting combat with %d combatants"), CombatantArray.Num());
	
	// Create temporary initiative holder's MAP
	TMap<float, TObjectPtr<ACombatant>> TempInitiativeHolder;
	// Loop through each combatant
	for (auto& Combatant : CombatantArray)//auto is just auto type finder for QOL 
	{
		if (!Combatant)
		{
			UE_LOG(Log_TurnManager, Error, TEXT("Found null combatant in array!"));
			continue;
		}
		
		// Roll initiative for this combatant
		float Initiative = Combatant->RollInitiative();
		UE_LOG(Log_TurnManager, Display, TEXT("%s rolled initiative: %.2f"), *Combatant->GetName(), Initiative);
		
		// TODO: GetIndex and SetUnitIndexOnGrid - maybe not required. Most likely no longer required
		TempInitiativeHolder.Add(Initiative, Combatant); // we add here to temp MAP
	}
	
	UE_LOG(Log_TurnManager, Display, TEXT("Clearing CombatantArray before rearanging them, had %d members"), CombatantArray.Num());
	CombatantArray.Empty();						// Clear the combatant array
	
	
	TArray<float> KeysToSort;				
	TempInitiativeHolder.GetKeys(KeysToSort); // Get all keys from temp map
	UE_LOG(Log_TurnManager, Display, TEXT("Got %d initiative keys to sort"), KeysToSort.Num());
	
	// Sort keys in descending order
	KeysToSort.Sort([](const float& A, const float& B) { return A > B; });
	
	// Rebuild combatant's array in sorted order
	UE_LOG(Log_TurnManager, Display, TEXT("InitiativeRolls (sorted):"));
	for (const float& Key : KeysToSort)
	{
		if (TObjectPtr<ACombatant>* FoundCombatant = TempInitiativeHolder.Find(Key))
		{
			CombatantArray.Add(*FoundCombatant);
			UE_LOG(Log_TurnManager, Display, TEXT("%s -> %.2f"), *(*FoundCombatant)->GetName(), Key);
		}
	}
	
	// Store initiative map for UI reference
	CurrentInitiativeHolder = TempInitiativeHolder;
	
	UE_LOG(Log_TurnManager, Display, TEXT("Final CombatantArray has %d members"), CombatantArray.Num());
	
	if (!CombatantArray.IsEmpty())
	{
		CurrentCombatant = CombatantArray[0];
		bIsCombatActive=true;
		UE_LOG(Log_TurnManager, Warning, TEXT("Combat started! First combatant: %s"), *CurrentCombatant->GetName());
		
		// Broadcast combat started event
		if (OnCombatStarted.IsBound())
		{
			TArray<ACombatant*> CombatantPtrs;
			for (auto& Combatant : CombatantArray)
			{
				CombatantPtrs.Add(Combatant.Get());
			}
			OnCombatStarted.Broadcast(CombatantPtrs);
		}
		
		CurrentCombatant->BeginTurn();
		
		// Broadcast turn changed event
		if (OnTurnChanged.IsBound())
		{
			OnTurnChanged.Broadcast(CurrentCombatant.Get());
		}
	}
	else
	{
		UE_LOG(Log_TurnManager, Error, TEXT("CombatantArray is empty after sorting! Combat failed to start."));
	}
}

void UTurnManagerComponent::OnActionSpentInCombatant(float ActionsLeft)
{
	// Only handle turn ending logic
	if (ActionsLeft <= 0.0f)
	{
		UE_LOG(Log_TurnManager, Warning, TEXT("ShouldEndTurn"));
		EndTurn();	
	}
}

void UTurnManagerComponent::EndTurn()
{
	if(bIsCombatActive==false){return;}//safety check 
	UE_LOG(Log_TurnManager, Display, TEXT("Ending turn with %d combatants"), CombatantArray.Num());
	
	if (CurrentCombatant){CurrentCombatant->EndTurnEffects();} //Applies to the end of turn effects on the combatant
	CombatantTurn++; if (CombatantTurn >= CombatantArray.Num()){CombatantTurn=0;} //returns the initiative to the start
	if(CombatantArray[CombatantTurn])
	{
		CurrentCombatant=CombatantArray[CombatantTurn];//Sets the combatant whose turn should be as active combatant
		CurrentCombatant->BeginTurn(); //Starts new combatants turn
		
		// Broadcast turn changed event to update UI
		if (OnTurnChanged.IsBound())
		{
			OnTurnChanged.Broadcast(CurrentCombatant.Get());
		}
	}
	UE_LOG(Log_TurnManager, Log, TEXT("EndTurn function ran"));
}
void UTurnManagerComponent::EndCombat()
{
	CombatantTurn=0;
	//TODO Destroying all active combatants.

	CombatantArray.Empty();
	CurrentInitiativeHolder.Empty();
	CurrentCombatant = nullptr;
	bIsCombatActive=false;
	
	// Broadcast combat ended event
	if (OnCombatEnded.IsBound())
	{
		OnCombatEnded.Broadcast();
	}
	
	UE_LOG(Log_TurnManager, Log, TEXT("EndCombat function ran"));
}
//
void UTurnManagerComponent::RemoveCombatant(ACombatant* CombatantToRemove)
{
	/*TODO Should do 3 things.
	 * 1. Take if from combatant array and remove it This way next turn will clean it up anyway.
	 * 2. Destroy it - Not by destroy actor, but using a death function - with its animations ans stuff
	 * 3. remove it from S_TileData - Unit 
	 * remove it from initiative tracker. As init tracker is ONE - just get a direct ref on it. Why not.
	 */

}

void UTurnManagerComponent::IncapacitateCombatant(ACombatant* CombatantToRemove)
{//TODO after player/nonplayer characters are established
}




