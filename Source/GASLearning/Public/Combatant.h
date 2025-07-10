// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "Combatant.generated.h"
class UTurnManagerComponent;
class AGrid;
class UPointLightComponent;
class UCapsuleComponent;
UCLASS()
class GASLEARNING_API ACombatant : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACombatant();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;
	// References
	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	TObjectPtr<AGrid> GridRef = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "References")
	TObjectPtr<UTurnManagerComponent> TurnManagerRef = nullptr;

	// Location
	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	FIntPoint LocationIndex = FIntPoint(-1, -1);

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	TArray<FIntPoint> Path;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	FTransform PreviousTransform;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	FTransform NextTransform;

	// Highlighter
	UPROPERTY(BlueprintReadWrite, Category = "Highlighter")
	bool bIsHovered = false;

	UPROPERTY(BlueprintReadWrite, Category = "Highlighter")
	bool bIsSelected = false;

	//PointLight for initiative turn
	UPROPERTY(BlueprintReadWrite, Category = "Highlighter")
	TObjectPtr<UPointLightComponent> InitiativeLight;
	// Initiative
	UPROPERTY(BlueprintReadWrite, Category = "Initiative")
	int32 Initiative = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Initiative")
	bool bDoesHaveReaction = false;

	UPROPERTY(BlueprintReadWrite, Category = "Initiative")
	int32 ActionsLeft = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Initiative")
	FText CharacterName;
	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCapsuleComponent> Capsule;
	// Conditions
	UPROPERTY(BlueprintReadWrite, Category = "Conditions")
	FGameplayTagContainer Conditions;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Initiative")
	float RollInitiative();
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void SpendReaction();
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void BeginTurn();
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void EndTurnEffects();

	
};
