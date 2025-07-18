// Fill out your copyright notice in the Description page of Project Settings.


#include "Combatant.h"
#include "TurnManagerComponent.h"
#include "Grid.h"
#include "MovementSplineComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "AbilitySystemComponent.h"
#include "../Public/GAS/CombatAttributeSet.h"
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
	//MovementSpline
	MovementSpline = CreateDefaultSubobject<UMovementSplineComponent>(TEXT("MovementSpline"));
	MovementSpline->SetupAttachment(Capsule);
	//GAS
	CombatAttributes = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));
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
	
	// Initialize MovementSpline GridRef
	if (MovementSpline && GridRef)
	{
		MovementSpline->GridRef = GridRef;
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
		// Use ApplyModToAttribute to trigger delegates properly
		AbilitySystemComponent->ApplyModToAttribute(CombatAttributes->GetReactionAvailableAttribute(),
			EGameplayModOp::Override, 0.0f);
	}
}

void ACombatant::CancelAllActiveAbilities()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToCancel;
	
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Spec.IsActive())
		{
			AbilitiesToCancel.Add(Spec.Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& HandleToCancel : AbilitiesToCancel)
	{
		AbilitySystemComponent->CancelAbilityHandle(HandleToCancel);
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
		// Use ApplyModToAttribute to trigger delegates properly
		AbilitySystemComponent->ApplyModToAttribute(CombatAttributes->GetMaxActionsAttribute(),
			EGameplayModOp::Override, ActionsToGive);
		AbilitySystemComponent->ApplyModToAttribute(CombatAttributes->GetActionsRemainingAttribute(),
			EGameplayModOp::Override, ActionsToGive);
		// Set reaction available
		AbilitySystemComponent->ApplyModToAttribute(CombatAttributes->GetReactionAvailableAttribute(),
			EGameplayModOp::Override, ActionsToGive > 0 ? 1.0f : 0.0f);
		CombatAttributes->OnActionsRemainingChanged.Broadcast(0, ActionsToGive);
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

	// Cancel all active abilities to clean up persistent states (tile highlights, etc.)
	CancelAllActiveAbilities();

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

void ACombatant::HandleHealthChange(float Magnitude,  float NewHealth)
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

void ACombatant::HandleReflexChange(float Magnitude, float NewReflex)
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

void ACombatant::HandleReactionAvailableChange(float Magnitude, float NewReactionAvailable)
{
	UE_LOG(LogGAS, Log, TEXT("Reaction available changed: %f"), NewReactionAvailable);
}

void ACombatant::HandleActionsRemainingChange(float Magnitude, float NewActionsRemaining)
{
	UE_LOG(LogGAS, Warning, TEXT("HandleActionsRemainingChange called - Magnitude: %f, NewActionsRemaining: %f"), Magnitude, NewActionsRemaining);
	// Inform TurnManager about action changes
	if (TurnManagerRef)
	{
		UE_LOG(LogGAS, Warning, TEXT("Calling TurnManager->OnActionSpentInCombatant with %d"), FMath::RoundToInt(NewActionsRemaining));
		TurnManagerRef->OnActionSpentInCombatant(FMath::RoundToInt(NewActionsRemaining));
	}
	else
	{
		UE_LOG(LogGAS, Error, TEXT("TurnManagerRef is null in HandleActionsRemainingChange!"));
	}
}

void ACombatant::SetCombatAttribute(ECombatAttributeType AttributeType, float Value)
{
	if (!CombatAttributes || !AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("SetCombatAttribute: CombatAttributes or ASC is null"));
		return;
	}
	
	// Use ApplyModToAttribute for proper GAS integration
	FGameplayAttribute Attribute;
	
	switch (AttributeType)
	{
		case ECombatAttributeType::Health:
			Attribute = CombatAttributes->GetHealthAttribute();
			break;
		case ECombatAttributeType::MaxHealth:
			Attribute = CombatAttributes->GetMaxHealthAttribute();
			break;
		case ECombatAttributeType::AC:
			Attribute = CombatAttributes->GetACAttribute();
			break;
		case ECombatAttributeType::Fortitude:
			Attribute = CombatAttributes->GetFortitudeAttribute();
			break;
		case ECombatAttributeType::Reflex:
			Attribute = CombatAttributes->GetReflexAttribute();
			break;
		case ECombatAttributeType::Will:
			Attribute = CombatAttributes->GetWillAttribute();
			break;
		case ECombatAttributeType::Perception:
			Attribute = CombatAttributes->GetPerceptionAttribute();
			break;
		case ECombatAttributeType::MovementSpeed:
			Attribute = CombatAttributes->GetMovementSpeedAttribute();
			break;
		case ECombatAttributeType::Initiative:
			Attribute = CombatAttributes->GetInitiativeAttribute();
			break;
		case ECombatAttributeType::ActionsRemaining:
			Attribute = CombatAttributes->GetActionsRemainingAttribute();
			break;
		case ECombatAttributeType::MaxActions:
			Attribute = CombatAttributes->GetMaxActionsAttribute();
			break;
		case ECombatAttributeType::ReactionAvailable:
			Attribute = CombatAttributes->GetReactionAvailableAttribute();
			break;
		case ECombatAttributeType::AttackBonus:
			Attribute = CombatAttributes->GetAttackBonusAttribute();
			break;
		case ECombatAttributeType::DamageBonus:
			Attribute = CombatAttributes->GetDamageBonusAttribute();
			break;
		case ECombatAttributeType::DamageDie:
			Attribute = CombatAttributes->GetDamageDieAttribute();
			break;
		case ECombatAttributeType::DamageDieCount:
			Attribute = CombatAttributes->GetDamageDieCountAttribute();
			break;
		case ECombatAttributeType::MaxDieRoll:
			Attribute = CombatAttributes->GetMaxDieRollAttribute();
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("SetCombatAttribute: Unknown attribute type"));
			return;
	}
	
	// Set the attribute value properly through GAS
	AbilitySystemComponent->SetNumericAttributeBase(Attribute, Value);
}

float ACombatant::GetCombatAttribute(ECombatAttributeType AttributeType) const
{
	if (!CombatAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("GetCombatAttribute: CombatAttributes is null"));
		return 0.0f;
	}
	
	switch (AttributeType)
	{
		case ECombatAttributeType::Health:
			return CombatAttributes->GetHealth();
		case ECombatAttributeType::MaxHealth:
			return CombatAttributes->GetMaxHealth();
		case ECombatAttributeType::AC:
			return CombatAttributes->GetAC();
		case ECombatAttributeType::Fortitude:
			return CombatAttributes->GetFortitude();
		case ECombatAttributeType::Reflex:
			return CombatAttributes->GetReflex();
		case ECombatAttributeType::Will:
			return CombatAttributes->GetWill();
		case ECombatAttributeType::Perception:
			return CombatAttributes->GetPerception();
		case ECombatAttributeType::MovementSpeed:
			return CombatAttributes->GetMovementSpeed();
		case ECombatAttributeType::Initiative:
			return CombatAttributes->GetInitiative();
		case ECombatAttributeType::ActionsRemaining:
			return CombatAttributes->GetActionsRemaining();
		case ECombatAttributeType::MaxActions:
			return CombatAttributes->GetMaxActions();
		case ECombatAttributeType::ReactionAvailable:
			return CombatAttributes->GetReactionAvailable();
		case ECombatAttributeType::AttackBonus:
			return CombatAttributes->GetAttackBonus();
		case ECombatAttributeType::DamageBonus:
			return CombatAttributes->GetDamageBonus();
		case ECombatAttributeType::DamageDie:
			return CombatAttributes->GetDamageDie();
		case ECombatAttributeType::DamageDieCount:
			return CombatAttributes->GetDamageDieCount();
		case ECombatAttributeType::MaxDieRoll:
			return CombatAttributes->GetMaxDieRoll();
		default:
			UE_LOG(LogTemp, Error, TEXT("GetCombatAttribute: Unknown attribute type"));
			return 0.0f;
	}
}

void ACombatant::InitializeCombatAttributes(const FS_CombatAttributes& CombatAttributesData)
{
	if (!CombatAttributes || !AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InitializeCombatAttributes: CombatAttributes or ASC is null"));
		return;
	}
	
	// Initialize attributes properly through GAS using InitStats
	CombatAttributes->InitMaxHealth(CombatAttributesData.MaxHealth);
	CombatAttributes->InitHealth(CombatAttributesData.MaxHealth); // Start at full health
	CombatAttributes->InitAC(CombatAttributesData.AC);
	CombatAttributes->InitFortitude(CombatAttributesData.Fortitude);
	CombatAttributes->InitReflex(CombatAttributesData.Reflex);
	CombatAttributes->InitWill(CombatAttributesData.Will);
	CombatAttributes->InitPerception(CombatAttributesData.Perception);
	CombatAttributes->InitMovementSpeed(CombatAttributesData.MovementSpeed);
	CombatAttributes->InitInitiative(CombatAttributesData.Perception); // Initiative = Perception in PF2e
	CombatAttributes->InitMaxActions(CombatAttributesData.MaxActions);
	CombatAttributes->InitActionsRemaining(CombatAttributesData.MaxActions); // Start with full actions
	CombatAttributes->InitReactionAvailable(1); // Start with reaction available
	CombatAttributes->InitAttackBonus(CombatAttributesData.AttackBonus);
	CombatAttributes->InitDamageBonus(CombatAttributesData.DamageBonus);
	CombatAttributes->InitDamageDie(CombatAttributesData.DamageDie);
	CombatAttributes->InitDamageDieCount(CombatAttributesData.DamageDieCount);
	CombatAttributes->InitMaxDieRoll(CombatAttributesData.MaxDieRoll);
	
	UE_LOG(LogTemp, Log, TEXT("Combat attributes initialized - Health: %f/%f, AC: %f, MaxDieRoll: %f"), 
		CombatAttributes->GetHealth(), CombatAttributes->GetMaxHealth(), 
		CombatAttributes->GetAC(), (float)CombatAttributes->GetMaxDieRoll());
}