#include "TurnManagerComponent.h"
#include "Grid.h"
#include "Combatant.h"
#include "LogTypes.h"
#include "StructsAndEnums/FS_IntPointArray.h"
#include "StructsAndEnums/FL_Spells_CPP.h"
#include "RangeFinder.h"
UTurnManagerComponent::UTurnManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

AGrid* UTurnManagerComponent::GetOwningGrid() const
{
	return Cast<AGrid>(GetOwner());
}

void UTurnManagerComponent::InitializeCombat()
{
	SpawnEnemies(); //This is currently implemented in blueprint
	//TODO in blueprint add the spawn logic to make them spawn with all assets
}
void UTurnManagerComponent::SetUnitOnGrid(ACombatant* Combatant, FIntPoint Index, bool Force)
{
	if (!Combatant || !GetOwningGrid()) return;
	
	// Check if we need to move the unit
	if (Combatant->LocationIndex != Index || Force)
	{
		AGrid* Grid = GetOwningGrid();
		
		// Clear unit from current tile
		if (FS_TileData* CurrentTile = Grid->GridTiles.Find(Combatant->LocationIndex))
		{
			CurrentTile->UnitOnTile = nullptr;
		}
		
		// Set unit on new tile
		if (FS_TileData* NewTile = Grid->GridTiles.Find(Index))
		{
			NewTile->UnitOnTile = Combatant;
			Combatant->LocationIndex = Index;
		}
	}
}
void UTurnManagerComponent::StartCombat() //StartInitiativeButtonActivates this.
{
	UE_LOG(Log_TurnManager, Warning, TEXT("StartCombat called"));
	
	// Check if combatant's array is not empty
	if (CombatantArray.IsEmpty()) 
	{
		UE_LOG(Log_TurnManager, Error, TEXT("CombatantArray is empty! Cannot start combat."));
		return;
	}
	
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

void UTurnManagerComponent::OnActionSpentInCombatant(int32 ActionsLeft)
{
	// Broadcast action change event for UI update
	if (OnCombatantActionsChanged.IsBound() && CurrentCombatant)
	{
		OnCombatantActionsChanged.Broadcast(CurrentCombatant.Get(), ActionsLeft);
	}
	
	if (ActionsLeft <= 0)	EndTurn();
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
//Spells 
TArray<FIntPoint> UTurnManagerComponent::GetSpellArea(FIntPoint Origin,
	FIntPoint CasterLocation)
{
	//TODO Requires F_SpellDataTargeting which requires AREA AND PATTERN ENUMS, BECAUSE FUCK ME THATS WHY
	//WHY THE FUCK I THOUGH MAKING A CRPG AS A SOLO WAS A GOOD IDEA? TODO MAKE AI make all those ENUMS AND STRUCTURES
	return {};
}

TArray<FIntPoint> UTurnManagerComponent::GetEffectRange(uint8 Range,
                                                        bool IgnoreLOS, bool IgnoreOrigin)
{
	AGrid* Grid = GetOwningGrid(); //probably should have made this a static property.//TODO cache GRID
	URangeFinder* Rangefinder =
Grid->GetComponentByClass<URangeFinder>();//TODO probably should Cache this too.
	if (!Rangefinder)
	{
		UE_LOG(LogTemp, Warning, TEXT("No RangeFinder component found on Grid"));
		return TArray<FIntPoint>();
	}
	//GENERATE POSSIBLE ARRAY
	
	FS_IntPointArray TempWrapper; //This is done because FS_IntPointArray isn't an int-point array. UGH.
	TempWrapper.IntPointArray = Rangefinder->GeneratePossibleArray(FIntPoint(0,0),
		CurrentCombatant->LocationIndex, Range,EAE_SpellPattern_Emanation, IgnoreOrigin);
	//We found this way the emanation around the player, which is most abilities range.
	
	EffectRangeTEMP = Grid->RemoveObstacleTiles(TempWrapper);
	//Remove Ones that are 'obstacle' or 'none' tiles.
	//Check if LOS is ignored. Should be false because 
	if (IgnoreLOS) //this is mostly a safety check.
	{	UE_LOG(Log_TurnManager, Error, TEXT("GetEffectRange in Turn manager component somehow ignores LOS WTF"));
		return{EffectRangeTEMP.IntPointArray};
	}
	EffectRangeTEMP =UFL_Spells_CPP::LineTraceSpells(EffectRangeTEMP, GetOwningGrid(), CurrentCombatant->LocationIndex);//

	UE_LOG(Log_TurnManager, Log, TEXT("LineTraceSpells was most likely used and cleared array is about to be returned"));
	return
	{
		EffectRangeTEMP.IntPointArray
	};
	
}


