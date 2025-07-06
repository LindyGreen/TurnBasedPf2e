// Fill out your copyright notice in the Description page of Project Settings.


#include "Combatant.h"
#include "TurnManagerComponent.h"
#include "Grid.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACombatant::ACombatant()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACombatant::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACombatant::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ACombatant::RollInitiative()
{
	return Initiative + UKismetMathLibrary::RandomFloatInRange(1.0f, 20.0f);
}

void ACombatant::SpendReaction()
{
	bDoesHaveReaction = false;
}



void ACombatant::BeginTurn()
{
	// Check if incapacitated - skip turn entirely
	if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Incapacitated")))
	{
		//TODO Put Death Saving Throws Here, when those will be designed
		EndTurnEffects(); 
		return;
	}

	// Default actions per turn
	int32 ActionsToGive = 3;

	// Check for condition modifiers
	FGameplayTagContainer ActionMods;
	ActionMods.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Stunned"));
	ActionMods.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Slowed"));
	ActionMods.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Quickened"));

	if (Conditions.HasAny(ActionMods))
	{
		// Check specific conditions and modify actions
		if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Stunned.1")))
		{
			ActionsToGive = 2;
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Stunned"));
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Stunned.2")))
		{
			ActionsToGive = 1;
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Stunned"));
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Stunned.3")))
		{
			ActionsToGive = 0;
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Stunned"));
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Slowed.1")))
		{
			ActionsToGive = 2;
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Slowed.2")))
		{
			ActionsToGive = 1;
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Quickened")))
		{
			ActionsToGive = 4;
		}
	}

	// Set actions and inform turn manager
	if (TurnManagerRef)
	{
		TurnManagerRef->CurrentTurnActions=ActionsToGive;
	}

	// Reset reaction unless no actions (then end turn immediately)
	if (ActionsToGive == 0 || Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Stunned")))
	{
		EndTurnEffects();
	}
	else
	{
		bDoesHaveReaction = true;
	}
}

void ACombatant::EndTurnEffects()
{
	// Define degrading condition tags
	FGameplayTagContainer DegradingConditions;
	DegradingConditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1"));
	DegradingConditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2"));
	DegradingConditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.3"));
	DegradingConditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1"));
	DegradingConditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2"));
	DegradingConditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.3"));
	DegradingConditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.1"));
	DegradingConditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.2"));
	DegradingConditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.3"));
	
	// Check if combatant has any degrading conditions
	if (Conditions.HasAny(DegradingConditions))
	{
		// Clumsy conditions
		if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1")))
		{
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1"));
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2")))
		{
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2"));
			Conditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1"));
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.3")))
		{
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.3"));
			Conditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2"));
		}
		
		// Enfeebled conditions
		if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1")))
		{
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1"));
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2")))
		{
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2"));
			Conditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1"));
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.3")))
		{
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.3"));
			Conditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2"));
		}
		
		// Frightened conditions
		if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.1")))
		{
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.1"));
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.2")))
		{
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.2"));
			Conditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.1"));
		}
		else if (Conditions.HasTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.3")))
		{
			Conditions.RemoveTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.3"));
			Conditions.AddTag(FGameplayTag::RequestGameplayTag("Conditions.Frightened.2"));
		}
	}
}

