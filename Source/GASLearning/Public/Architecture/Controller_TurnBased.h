// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "StructsAndEnums/E_TileState.h"
#include "Controller_TurnBased.generated.h"

class AGrid;
class UTurnManagerComponent;
class ACombatant;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredTileUpdated, FIntPoint, NewTileIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTileOrActorClicked, FIntPoint, TileIndex, AActor*, ClickedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRMB);

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
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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

	// Enhanced Input setup
	virtual void SetupInputComponent() override;

	// Input Action properties - set these in Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input") 
	TObjectPtr<class UInputAction> IALeftClick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<class UInputAction> IARightClick;

	// Enhanced Input callbacks
	void OnLeftMouseButtonPressed(const FInputActionValue& Value);
	void OnRightMouseButtonPressed(const FInputActionValue& Value);

	// Input delegates
	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnHoveredTileUpdated OnHoveredTileUpdated;
	
	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnTileOrActorClicked OnTileOrActorClicked;
	
	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnRMB OnRMB;

};
