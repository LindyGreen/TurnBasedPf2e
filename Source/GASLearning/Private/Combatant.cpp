// Fill out your copyright notice in the Description page of Project Settings.


#include "Combatant.h"
#include "TurnManagerComponent.h"
#include "Grid.h"
#include "GAS/MyBaseGameplayAbility.h"

#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "AbilitySystemComponent.h"
#include "../Public/GAS/CombatAttributeSet.h"
#include "../Public/GAS/SkillsAttributeSet.h"
#include "../Public/GAS/SpellsAttributeSet.h"
#include "StructsAndEnums/FS_Skills.h"
#include "StructsAndEnums/FS_SpellResources.h"
#include "LogTypes.h"

#include "Kismet/KismetMathLibrary.h"
#include "UI/DamageNumberWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"

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
	//GAS - All combatants get all attribute sets (unused ones stay at 0)
	CombatAttributes = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));
	SkillsAttributes = CreateDefaultSubobject<USkillsAttributeSet>(TEXT("SkillsAttributeSet"));
	SpellsAttributes = CreateDefaultSubobject<USpellsAttributeSet>(TEXT("SpellsAttributeSet"));
	
	//Damage Number Widget Component
	DamageNumberComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageNumberComponent"));
	DamageNumberComponent->SetupAttachment(Capsule);
	DamageNumberComponent->SetRelativeLocation(FVector(0, 0, 100)); // 100 units above
	DamageNumberComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DamageNumberComponent->SetTwoSided(true);
	DamageNumberComponent->SetDrawSize(FVector2D(200, 100));
	DamageNumberComponent->SetVisibility(false); // Hidden by default
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
	
	// Initialize damage number widget component if class is set
	if (DamageNumberComponent && DamageNumberWidgetClass)
	{
		DamageNumberComponent->SetWidgetClass(DamageNumberWidgetClass);
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

	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);

	// Check if incapacitated - skip turn entirely
	if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Incapacitated")))
	{
		//TODO Put Death Saving Throws Here, when those will be designed
		EndTurnEffects();
		return;
	}
	InitiativeLight->SetHiddenInGame(false); //INITIATIVE LIGHT
	// Default actions per turn
	int32 ActionsToGive = 3;

	// Check for condition modifiers using ASC
	if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Stunned.1")))
	{
		ActionsToGive = 2;
		AbilitySystemComponent->RemoveLooseGameplayTag(
			FGameplayTag::RequestGameplayTag("Conditions.Stunned.1"));
	}
	else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Stunned.2")))
	{
		ActionsToGive = 1;
		AbilitySystemComponent->RemoveLooseGameplayTag(
			FGameplayTag::RequestGameplayTag("Conditions.Stunned.2"));
	}
	else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Stunned.3")))
	{
		ActionsToGive = 0;
		AbilitySystemComponent->RemoveLooseGameplayTag(
			FGameplayTag::RequestGameplayTag("Conditions.Stunned.3"));
	}
	else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Slowed.1")))
	{
		ActionsToGive = 2;
	}
	else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Slowed.2")))
	{
		ActionsToGive = 1;
	}
	else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Quickened")))
	{
		ActionsToGive = 4;
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

	// End turn immediately if no actions or stunned
	if (ActionsToGive == 0 || OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Stunned.3")))
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
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.MAP.1"));
	DegradingConditions.AddTag(
		FGameplayTag::RequestGameplayTag("Conditions.MAP.2"));

	// Check ASC for degrading conditions and handle them
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer OwnedTags;
		AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);
		
		// MAP removal from ASC
		if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.MAP.1")))
			AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("Conditions.MAP.1"));
		if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.MAP.2")))
			AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("Conditions.MAP.2"));
		
		// Clumsy conditions
		if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1")))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1"));
		}
		else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2")))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2"));
			AbilitySystemComponent->AddLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Clumsy.1"));
		}
		else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Clumsy.3")))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Clumsy.3"));
			AbilitySystemComponent->AddLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Clumsy.2"));
		}

		// Enfeebled conditions
		if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1")))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1"));
		}
		else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2")))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2"));
			AbilitySystemComponent->AddLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.1"));
		}
		else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.3")))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.3"));
			AbilitySystemComponent->AddLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Enfeebled.2"));
		}

		// Frightened conditions
		if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Frightened.1")))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Frightened.1"));
		}
		else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Frightened.2")))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Frightened.2"));
			AbilitySystemComponent->AddLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Frightened.1"));
		}
		else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Conditions.Frightened.3")))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Frightened.3"));
			AbilitySystemComponent->AddLooseGameplayTag(
				FGameplayTag::RequestGameplayTag("Conditions.Frightened.2"));
		}
	}
}
#pragma region CombatAttributeSet handlers

void ACombatant::HandleHealthChange(float Magnitude,  float NewHealth)
{
	// Show damage number if there was health change
	if (Magnitude != 0.0f)
	{
		ShowDamageNumber(Magnitude);
	}
	
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

void ACombatant::InitializeAttributes(const FS_CombatAttributes& CombatAttributesData, const FS_Skills& SkillsData, const FS_SpellResourcesAndAttributes& SpellsData)
{
	if (!CombatAttributes || !SkillsAttributes || !SpellsAttributes || !AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InitializeAttributes: One or more attribute sets or ASC is null"));
		return;
	}
	
	// Initialize Combat attributes
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
	
	// Initialize Skills attributes (all PF2e skills)
	SkillsAttributes->InitAcrobatics(SkillsData.Acrobatics);
	SkillsAttributes->InitArcana(SkillsData.Arcana);
	SkillsAttributes->InitAthletics(SkillsData.Athletics);
	SkillsAttributes->InitCrafting(SkillsData.Crafting);
	SkillsAttributes->InitDeception(SkillsData.Deception);
	SkillsAttributes->InitDiplomacy(SkillsData.Diplomacy);
	SkillsAttributes->InitIntimidation(SkillsData.Intimidation);
	SkillsAttributes->InitMedicine(SkillsData.Medicine);
	SkillsAttributes->InitNature(SkillsData.Nature);
	SkillsAttributes->InitOccultism(SkillsData.Occultism);
	SkillsAttributes->InitPerformance(SkillsData.Performance);
	SkillsAttributes->InitReligion(SkillsData.Religion);
	SkillsAttributes->InitSociety(SkillsData.Society);
	SkillsAttributes->InitStealth(SkillsData.Stealth);
	SkillsAttributes->InitSurvival(SkillsData.Survival);
	SkillsAttributes->InitThievery(SkillsData.Thievery);
	
	// Initialize Spells attributes (spell slots, focus points, etc.)
	SpellsAttributes->InitSpellAttackBonus(SpellsData.SpellAttackBonus);
	SpellsAttributes->InitSpellSaveDC(SpellsData.SpellSaveDC);
	SpellsAttributes->InitIsSpontaneous(SpellsData.bIsSpontaneous ? 1.0f : 0.0f);
	SpellsAttributes->InitPreparableCantrips(SpellsData.PreperableCantrips);
	
	// Level 1 slots
	SpellsAttributes->InitLevel1Slots(SpellsData.Level1Slots);
	SpellsAttributes->InitLevel1Slot1(SpellsData.Level1Slot1 ? 1.0f : 0.0f);
	SpellsAttributes->InitLevel1Slot2(SpellsData.Level1Slot2 ? 1.0f : 0.0f);
	SpellsAttributes->InitLevel1Slot3(SpellsData.Level1Slot3 ? 1.0f : 0.0f);
	
	// Level 2 slots
	SpellsAttributes->InitLevel2Slots(SpellsData.Level2Slots);
	SpellsAttributes->InitLevel2Slot1(SpellsData.Level2Slot1 ? 1.0f : 0.0f);
	SpellsAttributes->InitLevel2Slot2(SpellsData.Level2Slot2 ? 1.0f : 0.0f);
	SpellsAttributes->InitLevel2Slot3(SpellsData.Level2Slot3 ? 1.0f : 0.0f);
	
	// Level 3 slots
	SpellsAttributes->InitLevel3Slots(SpellsData.Level3Slots);
	SpellsAttributes->InitLevel3Slot1(SpellsData.Level3Slot1 ? 1.0f : 0.0f);
	SpellsAttributes->InitLevel3Slot2(SpellsData.Level3Slot2 ? 1.0f : 0.0f);
	SpellsAttributes->InitLevel3Slot3(SpellsData.Level3Slot3 ? 1.0f : 0.0f);
	
	// Divine/Focus
	SpellsAttributes->InitDivineFont(SpellsData.DivineFont);
	SpellsAttributes->InitMaxFocusPoints(SpellsData.MaxFocusPoints);
	SpellsAttributes->InitCurrentFocusPoints(SpellsData.MaxFocusPoints); // Start at max focus points
	
	UE_LOG(LogTemp, Log, TEXT("All attributes initialized - Health: %f/%f, AC: %f, Stealth: %f, Spell DC: %f"), 
		CombatAttributes->GetHealth(), CombatAttributes->GetMaxHealth(), 
		CombatAttributes->GetAC(), SkillsAttributes->GetStealth(), SpellsAttributes->GetSpellSaveDC());
}

void ACombatant::InitStartingEffectsAndAbilities(const TArray<TSubclassOf<UMyBaseGameplayAbility>>& StartingAbilities, const TArray<TSubclassOf<UGameplayEffect>>& StartingEffects)
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InitStartingEffectsAndAbilities: ASC is null"));
		return;
	}

	// Grant starting abilities
	for (const TSubclassOf<UMyBaseGameplayAbility>& AbilityClass : StartingAbilities)
	{
		if (AbilityClass)
		{
			FGameplayAbilitySpec AbilitySpec(TSubclassOf<UGameplayAbility>(AbilityClass), 1, INDEX_NONE);
			AbilitySystemComponent->GiveAbility(AbilitySpec);
		}
	}

	// Apply starting effects (resistances, buffs, etc.)
	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartingEffects)
	{
		if (EffectClass)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			
			FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.0f, EffectContext);
			if (EffectSpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
				UE_LOG(LogTemp, Log, TEXT("Applied starting effect: %s"), *EffectClass->GetName());
			}
		}
	}
}

void ACombatant::GenerateMovementPath(const TArray<FIntPoint>& PathIndices)
{
	if (!GridRef)
	{
		return;
	}

	FVector CurrentLocation = GetActorLocation();
	float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	
	GridRef->GenerateMovementPath(CurrentLocation, PathIndices, CapsuleHalfHeight);
}

// Simple Look-At System
void ACombatant::EnableLookAt()
{
	OriginalRotation = GetActorRotation();
	bShouldLookAt = true;
}

void ACombatant::DisableLookAt()
{
	bShouldLookAt = false;
}

void ACombatant::SetLookAtLocation(FVector TargetLocation)
{
	LookAtTargetLocation = TargetLocation;
}

void ACombatant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Simple look-at rotation (yaw only)
	if (bShouldLookAt)
	{
		FVector Direction = (LookAtTargetLocation - GetActorLocation()).GetSafeNormal();
		FRotator TargetRotation = Direction.Rotation();
		TargetRotation.Pitch = 0.0f; // Only yaw rotation
		TargetRotation.Roll = 0.0f;
		
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, LookAtSpeed);
		SetActorRotation(NewRotation);
	}
}

void ACombatant::ShowDamageNumber(float DamageAmount)
{
	if (!DamageNumberComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageNumberComponent is null on %s"), *GetName());
		return;
	}

	// Get the widget from the component
	UDamageNumberWidget* DamageWidget = Cast<UDamageNumberWidget>(DamageNumberComponent->GetWidget());
	if (!DamageWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageNumberWidget is null on %s"), *GetName());
		return;
	}

	// Show the component and update damage
	DamageNumberComponent->SetVisibility(true);
	
	// Get world location (the component is already positioned correctly)
	FVector WorldLocation = DamageNumberComponent->GetComponentLocation();
	DamageWidget->ShowDamage(DamageAmount, WorldLocation);

	// Set up auto-hide timer
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (DamageNumberComponent && IsValid(DamageNumberComponent))
		{
			DamageNumberComponent->SetVisibility(false);
		}
	}, 2.0f, false);

	UE_LOG(LogTemp, Log, TEXT("Showed damage number: %f for %s"), DamageAmount, *GetName());
}
