// Fill out your copyright notice in the Description page of Project Settings.


#include "Architecture/Controller_TurnBased.h"
#include "Combatant.h"
#include "LogTypes.h" 
#include "Grid.h"
#include "TurnManagerComponent.h"
#include "Kismet/GameplayStatics.h"

AController_TurnBased::AController_TurnBased()
{
	// Enable input by default
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AController_TurnBased::BeginPlay()
{
	Super::BeginPlay();
	
	// Find the Grid actor in the world
	if (UWorld* World = GetWorld())
	{
		GridRef = Cast<AGrid>(UGameplayStatics::GetActorOfClass(World, AGrid::StaticClass()));
		if (!GridRef)
		{
			UE_LOG(LogTemp, Warning, TEXT("AController_TurnBased: Grid not found in world!"));
		}
		else
		{
			// Get TurnManager component from Grid
			TurnManagerRef = GridRef->FindComponentByClass<UTurnManagerComponent>();
			if (!TurnManagerRef)
			{
				UE_LOG(LogTemp, Warning, TEXT("AController_TurnBased: TurnManager component not found on Grid!"));
			}
		}
	}
}

void AController_TurnBased::UpdateTileUnderCursor()
{
AActor* CurrentHover = GetUnitUnderCursor();//setting the one we connect with as current hover
	if (CurrentHover)//check  if found anything.
{
//		UE_LOG(Log_Grid, Log, TEXT("Valid CurentHover- GetUnitUnderCursor()"));
		
		if (HoveredCombatant)//was a combatant hovered before
		{
//		UE_LOG(Log_Grid, Log, TEXT("Hovered combatant is valid under said cursor"));
			if (!(CurrentHover==HoveredCombatant))//checks if the current hover is the hoveredCombatant
//		UE_LOG(Log_Grid, Log, TEXT("Hovered combatant != Current hover"));
			{
			// Call ChangeIsHovered in previous combatant - false
			if (HoveredCombatant)
			{
				HoveredCombatant->SetIsHovered(false);
			}
			HoveredCombatant =Cast<ACombatant>(CurrentHover);//set current hover as the HoveredCombatant and make previous unhover
			// Call ChangeIsHovered in new combatant - true
			if (HoveredCombatant)
			{
				HoveredCombatant->SetIsHovered(true);
			}
			}		
		}
		else //no active hovered combatant before, one now
		{
//		UE_LOG(Log_Grid, Log, TEXT("HoveredCombatant is not valid - there is't one likely"));
		HoveredCombatant =Cast<ACombatant>(CurrentHover);//set current hover as the HoveredCombatant
		// Call ChangeIsHovered in combatant - true
		if (HoveredCombatant)
		{
			HoveredCombatant->SetIsHovered(true);
		}
		}
}

else //clear HoveredCombatant
{
//	UE_LOG(Log_Grid, Log, TEXT("There isn't a combatant under mouse, so we check to clear if there is one"));
	
	if (HoveredCombatant)
	{
//	UE_LOG(Log_Grid, Log, TEXT("We clear previous hovered combatant"));
		// Call ChangeIsHoveredInCombatant-false
		HoveredCombatant->SetIsHovered(false);
		HoveredCombatant=nullptr;
	}
	
}
	FIntPoint NewIndex;
	if (HoveredCombatant)
	{
//	UE_LOG(Log_Grid, Log, TEXT("new Hovered Combatant we have to get Index From"));
	NewIndex=HoveredCombatant->LocationIndex;
	}
	else//if no combatant under the cursor
	{
//		UE_LOG(Log_Grid, Log, TEXT("we point at an empty grid"));
		if (GridRef)
		{
//		UE_LOG(Log_Grid, Log, TEXT("we get tile under cursor"));
			NewIndex=GridRef->GetTileIndexUnderCursor();
		}
	}
	if (NewIndex!=HoveredTileIndex)//changing the index of the hovered tile to change it's state
	{
//	UE_LOG(Log_Grid, Log, TEXT("we switched from one hovered tile to another"));
		if (GridRef && HoveredTileIndex != FIntPoint(-1, -1))
		{
			GridRef->RemoveStateFromTile(HoveredTileIndex, ETileState::Hovered);
		}
		HoveredTileIndex=NewIndex;
		if (GridRef && HoveredTileIndex != FIntPoint(-1, -1))
		{
			GridRef->AddStateToTile(HoveredTileIndex, ETileState::Hovered);
		}
	}
	
}//end of updateTileUnderCursor



AActor* AController_TurnBased::GetUnitUnderCursor()
{
	FHitResult HitResult;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3), true, HitResult))
	{
		return HitResult.GetActor();
	}
	
	// Fallback: try to get unit from grid tile in case you select the grid, but not the combatant
	if (GridRef)
	{
		FIntPoint TileIndex = GridRef->GetTileIndexUnderCursor();
		if (FS_TileData* TileData = GridRef->GridTiles.Find(TileIndex))
		{
			return TileData->UnitOnTile;
		}
	}
	return nullptr;
}
