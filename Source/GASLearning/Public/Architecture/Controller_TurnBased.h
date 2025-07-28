// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StructsAndEnums/E_TileState.h"
#include "Controller_TurnBased.generated.h"

class AGrid;
class UTurnManagerComponent;
class ACombatant;
/**
 * 
 */
UCLASS()
class GASLEARNING_API AController_TurnBased : public APlayerController
{
	GENERATED_BODY()

public:
	// Constructor
	AController_TurnBased();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Grid reference for player actions
	UPROPERTY(BlueprintReadWrite, Category = "References")
	TObjectPtr<AGrid> GridRef = nullptr;

	// TurnManager reference for turn-based actions
	UPROPERTY(BlueprintReadWrite, Category = "References")
	TObjectPtr<UTurnManagerComponent> TurnManagerRef = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "References")
	TObjectPtr<ACombatant> HoveredCombatant = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "References")
	FIntPoint HoveredTileIndex={-1,-1};
	//Reference Setter
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SetGridReference(AActor* NewGridRef);

	// Input handling functions
	UFUNCTION(BlueprintCallable, Category = "Input")
	void UpdateTileUnderCursor(bool& WasHoverUpdated);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	AActor* GetUnitUnderCursor();

};
