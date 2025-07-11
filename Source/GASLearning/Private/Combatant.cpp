// Fill out your copyright notice in the Description page of Project Settings.


#include "Combatant.h"
#include "TurnManagerComponent.h"
#include "Grid.h"

#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "AbilitySystemComponent.h"

#include "GAS/CombatAttributeSet.h"
#include "LogTypes.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACombatant::ACombatant()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(
		TEXT("AbilitySystemComponent"));
	//Capsule
	Capsule = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	RootComponent=Capsule;
	//InitiativeLight
	InitiativeLight = CreateDefaultSubobject<UPointLightComponent>(
		"InitiativeLight");
	InitiativeLight->SetupAttachment(Capsule);
	InitiativeLight->LightColor = FColor(255, 0, 0);
	InitiativeLight->SetRelativeLocation(FVector(0, 0, 200));
	InitiativeLight->SetHiddenInGame(true);
	//GAS
	CombatAttributes = CreateDefaultSubobject<UCombatAttributeSet>(
		TEXT("CombatAttributeSet"));
}

// Called when the game starts or when spawned
void ACombatant::BeginPlay()
{
	Super::BeginPlay();
	if (AbilitySystemComponent) //Initialize Ability system Component - GAS.
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	if (CombatAttributes)
	{
		CombatAttributes->OnHealthChanged.AddDynamic(this,&ACombatant::HandleHealthChange);
		CombatAttributes->OnACChanged.AddDynamic(this,&ACombatant::HandleACChange);

		CombatAttributes->OnFortitudeChanged.AddDynamic(this,&ACombatant::HandleFortitudeChange);
		CombatAttributes->OnReflexChanged.AddDynamic(this,&ACombatant::HandleReflexChange);
		CombatAttributes->OnWillChanged.AddDynamic(this, &ACombatant::HandleWillChange);
		CombatAttributes->OnActionsRemainingChanged.AddDynamic(this, &ACombatant::HandleActionsRemainingChange);
		CombatAttributes->OnReactionAvailableChanged.AddDynamic(this, &ACombatant::HandleReactionAvailableChange);
	}
}

UAbilitySystemComponent* ACombatant::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


// Called to bind functionality to input
void ACombatant::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ACombatant::RollInitiative()
{
	// Now uses GAS Initiative attribute
	if (CombatAttributes)
	{
		return CombatAttributes->GetInitiative() + UKismetMathLibrary::RandomFloatInRange(1.0f, 20.0f);
	}
	return UKismetMathLibrary::RandomFloatInRange(1.0f, 20.0f);
}

void ACombatant::SpendReaction()
{
	// Now uses GAS ReactionAvailable attribute
	if (CombatAttributes && AbilitySystemComponent)
	{
		// Apply a GameplayEffect to set ReactionAvailable to 0
		// For now, set it directly (should use GameplayEffect in real implementation)
		CombatAttributes->SetReactionAvailable(0);
	}
}


void ACombatant::BeginTurn()
{
	// Check if incapacitated - skip turn entirely
	if (Conditions.HasTag(
		FGameplayTag::RequestGameplayTag("Conditions.Incapacitated")))
	{
		//TODO Put Death Saving Throws Here, when those will be designed
		EndTurnEffects();
		return;
	}
	InitiativeLight->SetHiddenInGame(false); //INITIATIVE LIGHT
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
		if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Stunned.1")))
		{
			ActionsToGive = 2;
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Stunned"));
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Stunned.2")))
		{
			ActionsToGive = 1;
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Stunned"));
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Stunned.3")))
		{
			ActionsToGive = 0;
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Stunned"));
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Slowed.1")))
		{
			ActionsToGive = 2;
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Slowed.2")))
		{
			ActionsToGive = 1;
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Quickened")))
		{
			ActionsToGive = 4;
		}
	}

	// Set actions in GAS - TurnManager will be informed via delegate
	if (CombatAttributes && AbilitySystemComponent)
	{
		// Set MaxActions first, then ActionsRemaining
		CombatAttributes->SetMaxActions(ActionsToGive);
		CombatAttributes->SetActionsRemaining(ActionsToGive);
		// Set reaction available
		CombatAttributes->SetReactionAvailable(ActionsToGive > 0 ? 1 : 0);
		
	}

	// End turn immediately if no actions
	if (ActionsToGive == 0 || Conditions.HasTag(
		FGameplayTag::RequestGameplayTag("Conditions.Stunned")))
	{
		EndTurnEffects();
	}
}

void ACombatant::EndTurnEffects()
{
	InitiativeLight->SetHiddenInGame(true);

	// Define degrading condition tags
	FGameplayTagContainer DegradingConditions;
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1"));
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2"));
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.Clumsy.3"));
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1"));
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2"));
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.3"));
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.Frightened.1"));
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.Frightened.2"));
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.Frightened.3"));

	// Check if combatant has any degrading conditions
	if (Conditions.HasAny(DegradingConditions))
	{
		// Clumsy conditions
		if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1")))
		{
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1"));
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2")))
		{
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2"));
			Conditions.AddTag(
				FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1"));
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Clumsy.3")))
		{
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Clumsy.3"));
			Conditions.AddTag(
				FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2"));
		}

		// Enfeebled conditions
		if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1")))
		{
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1"));
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2")))
		{
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2"));
			Conditions.AddTag(
				FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1"));
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.3")))
		{
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.3"));
			Conditions.AddTag(
				FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2"));
		}

		// Frightened conditions
		if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Frightened.1")))
		{
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Frightened.1"));
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Frightened.2")))
		{
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Frightened.2"));
			Conditions.AddTag(
				FGameplayTag::RequestGameplayTag("Conditions.Frightened.1"));
		}
		else if (Conditions.HasTag(
			FGameplayTag::RequestGameplayTag("Conditions.Frightened.3")))
		{
			Conditions.RemoveTag(
				FGameplayTag::RequestGameplayTag("Conditions.Frightened.3"));
			Conditions.AddTag(
				FGameplayTag::RequestGameplayTag("Conditions.Frightened.2"));
		}
	}
}
#pragma region CombatAttributeSet handlers
void ACombatant::HandleHealthChange(float Magnitude, 
 float NewHealth)
{
	if (NewHealth <= 0)
	{
		//TODO Handle death
				UE_LOG(LogGAS, Warning, TEXT("Unit died!"));
	}
	UE_LOG(LogGAS, Log, TEXT("Health changed: %f"),NewHealth);
}

void ACombatant::HandleACChange(float Magnitude, float NewAC)
{
	UE_LOG(LogGAS, Log, TEXT("AC changed: %f"),NewAC);
}

void ACombatant::HandleFortitudeChange(float Magnitude, float NewFortitude)
{
	UE_LOG(LogGAS, Log, TEXT("Fortitude changed: %f"), NewFortitude);
}

void ACombatant::HandleReflexChange(float Magnitude, 
float NewReflex)
{
	UE_LOG(LogGAS, Log, TEXT("Reflex changed: %f"),NewReflex);
}

void ACombatant::HandleWillChange(float Magnitude, float NewWill)
{
	UE_LOG(LogGAS, Log, TEXT("Will changed: %f"),NewWill);
}
#pragma endregion CombatAttributeSet handlers

void ACombatant::SetIsHovered(bool bNewIsHovered)
{
	bIsHovered = bNewIsHovered;
	CallToUpdateVisualIfHoveredOrSelected();
}

void ACombatant::SetIsSelected(bool bNewIsSelected)
{
	bIsSelected = bNewIsSelected;
	CallToUpdateVisualIfHoveredOrSelected();
}

void ACombatant::HandleActionsRemainingChange(float Magnitude, float NewActionsRemaining)
{
	UE_LOG(LogGAS, Log, TEXT("Actions remaining changed: %f"), NewActionsRemaining);
	// Inform TurnManager about action changes
	if (TurnManagerRef)
	{
		TurnManagerRef->OnActionSpentInCombatant(NewActionsRemaining);
	}
}

void ACombatant::HandleReactionAvailableChange(float Magnitude, float NewReactionAvailable)
{
	UE_LOG(LogGAS, Log, TEXT("Reaction available changed: %f"), NewReactionAvailable);
}

void ACombatant::SpendAction(int32 ActionCost)
{
	if (CombatAttributes && AbilitySystemComponent)
	{
		// Apply cost via attribute modifier
		AbilitySystemComponent->ApplyModToAttribute(CombatAttributes->GetActionsRemainingAttribute(),
			EGameplayModOp::Additive,-ActionCost);
	}
}

#pragma region Blueprint-callable getters
float ACombatant::GetActionsRemaining() const
{
	return CombatAttributes ? CombatAttributes->GetActionsRemaining() : 0.0f;
}

float ACombatant::GetMaxActions() const
{
	return CombatAttributes ? CombatAttributes->GetMaxActions() : 0.0f;
}

bool ACombatant::GetReactionAvailable() const
{
	return CombatAttributes ? CombatAttributes->GetReactionAvailable() > 0.0f : false;
}

float ACombatant::GetCurrentHealth() const
{
	return CombatAttributes ? CombatAttributes->GetHealth() : 0.0f;
}

float ACombatant::GetMaxHealth() const
{
	return CombatAttributes ? CombatAttributes->GetMaxHealth() : 0.0f;
}
#pragma endregion Blueprint-callable getters
