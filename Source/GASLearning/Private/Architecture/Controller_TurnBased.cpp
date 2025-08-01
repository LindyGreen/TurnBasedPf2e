// Fill out your copyright notice in the Description page of Project Settings.


#include "Architecture/Controller_TurnBased.h"
#include "Combatant.h"
#include "LogTypes.h" 
#include "Grid.h"
#include "TurnManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "InputAction.h"

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
	{
	EnableInput(this);
	}
	
	// Set input mode to allow UI and game input
	SetInputMode(FInputModeGameAndUI());
	
	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void AController_TurnBased::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Call UpdateTileUnderCursor every frame
	bool WasHoverUpdated = false;
	UpdateTileUnderCursor(WasHoverUpdated);
}

void AController_TurnBased::SetGridReference(AActor* NewGridActor)
{
	GridRef = Cast<AGrid>(NewGridActor);
	if (GridRef)
	{
		TurnManagerRef =GridRef->FindComponentByClass<UTurnManagerComponent>();
		// Any other grid-dependent setup
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetGridReference: Invalid Grid actor passed"));
	}
}


void AController_TurnBased::UpdateTileUnderCursor(bool& WasHoverUpdated)
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
	if (NewIndex==HoveredTileIndex){WasHoverUpdated=false;}

	if (NewIndex!=HoveredTileIndex)//changing the index of the hovered tile to change it's state
	{
		WasHoverUpdated=true;
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
		
		// Broadcast the tile hover change
		OnHoveredTileUpdated.Broadcast(HoveredTileIndex);
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

// Enhanced Input setup
void AController_TurnBased::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Cast to Enhanced Input Component
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Bind Enhanced Input Actions
		if (IALeftClick)
		{
			EnhancedInputComponent->BindAction(IALeftClick, ETriggerEvent::Triggered, this, &AController_TurnBased::OnLeftMouseButtonPressed);
		}
		
		if (IARightClick) 
		{
			EnhancedInputComponent->BindAction(IARightClick, ETriggerEvent::Triggered, this, &AController_TurnBased::OnRightMouseButtonPressed);
		}
	}
}

// Enhanced Input callbacks
void AController_TurnBased::OnLeftMouseButtonPressed(const FInputActionValue& Value)
{
	// Get the actor under cursor and current tile index
	AActor* ClickedActor = GetUnitUnderCursor();
	FIntPoint ClickedTileIndex = HoveredTileIndex;
	
	// Broadcast the click event with both tile and actor info
	OnTileOrActorClicked.Broadcast(ClickedTileIndex, ClickedActor);
}

void AController_TurnBased::OnRightMouseButtonPressed(const FInputActionValue& Value)
{
	// Broadcast right mouse button event
	OnRMB.Broadcast();
}
