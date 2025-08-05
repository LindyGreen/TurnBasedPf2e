// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "StructsAndEnums/ECombatAttributeType.h"
#include "StructsAndEnums/FS_CombatAttributes.h"
#include "StructsAndEnums/FS_Skills.h"
#include "StructsAndEnums/FS_SpellResources.h"
#include "StructsAndEnums/E_TileType.h"
#include "Combatant.generated.h"
class UTurnManagerComponent;
class AGrid;
class UPointLightComponent;
class UCapsuleComponent;
class UAbilitySystemComponent;
class UCombatAttributeSet;
class USkillsAttributeSet;
class USpellsAttributeSet;
class UMyBaseGameplayAbility;
class UGameplayEffect;
class UDamageNumberWidget;
class UWidgetComponent;

UCLASS()
class GASLEARNING_API ACombatant : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACombatant();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;//returns my ability component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;//requied to use gas

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;
	// References
	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	TObjectPtr<AGrid> GridRef = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "References")
	TObjectPtr<UTurnManagerComponent> TurnManagerRef = nullptr;

	// Location
	UPROPERTY(BlueprintReadWrite, Category = "Grid")//set in Turn manager SetUnitOnGrid
	FIntPoint LocationIndex = FIntPoint(-1, -1);

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	TArray<FIntPoint> Path;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	FTransform PreviousTransform;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	FTransform NextTransform;

	// Grid Spawner Name for enemy data table lookup
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn=true), Category = "Spawning")
	FName EnemiesOnMapRowName;

	// Movement constraints - what tile types this combatant can move through
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<ETileType> AccessibleTiles;

	// Highlighter
	UPROPERTY(BlueprintReadWrite, Category = "Highlighter")
	bool bIsHovered = false;

	UPROPERTY(BlueprintReadWrite, Category = "Highlighter")
	bool bIsSelected = false;

	//PointLight for initiative turn
	UPROPERTY(BlueprintReadWrite, Category = "Highlighter")
	TObjectPtr<UPointLightComponent> InitiativeLight;
	// Initiative, Actions, and Reaction are now in GAS CombatAttributeSet

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn=true), Category = "Initiative")
	FText CharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Initiative")
	TObjectPtr<UTexture2D> CharacterPortrait;
	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCapsuleComponent> Capsule;
	
	//GAS components - All combatants get all attribute sets
	UPROPERTY()
	TObjectPtr<UCombatAttributeSet> CombatAttributes;
	
	UPROPERTY()
	TObjectPtr<USkillsAttributeSet> SkillsAttributes;
	
	UPROPERTY()
	TObjectPtr<USpellsAttributeSet> SpellsAttributes;

	// Conditions
	UPROPERTY(BlueprintReadWrite, Category = "Conditions")
	FGameplayTagContainer Conditions;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Initiative")
	float RollInitiative();
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void SpendReaction();
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void CancelAllActiveAbilities();
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void BeginTurn();
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void EndTurnEffects();
	
	
	// GAS Wrapper Functions for Blueprint Access
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetCombatAttribute(ECombatAttributeType AttributeType, float Value);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
	float GetCombatAttribute(ECombatAttributeType AttributeType) const;
	
	// Bulk initialization from complete character data (combat, skills, spells)
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void InitializeAttributes(const FS_CombatAttributes& CombatAttributesData, const FS_Skills& SkillsData, const FS_SpellResourcesAndAttributes& SpellsData);

	// Initialize starting GameplayAbilities and GameplayEffects for this combatant
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void InitStartingEffectsAndAbilities(const TArray<TSubclassOf<UMyBaseGameplayAbility>>& StartingAbilities, const TArray<TSubclassOf<UGameplayEffect>>& StartingEffects);

#pragma region CombatAttributeSet handlers
	UFUNCTION(BlueprintCallable, Category = "Health")
	void HandleHealthChange(float Magnitude, float NewHealth);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleACChange(float Magnitude, float NewAC);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleFortitudeChange(float Magnitude, float NewFortitude);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleReflexChange(float Magnitude, float NewReflex);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleWillChange(float Magnitude, float NewWill);
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void HandleActionsRemainingChange(float Magnitude, float NewActionsRemaining);
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void HandleReactionAvailableChange(float Magnitude, float NewReactionAvailable);
#pragma endregion CombatAttributeSet handlers

	// Hover and Selection System
	UFUNCTION(BlueprintCallable, Category = "Highlighter")
	void SetIsHovered(bool bNewIsHovered);
	
	UFUNCTION(BlueprintCallable, Category = "Highlighter") 
	void SetIsSelected(bool bNewIsSelected);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Highlighter")
	void CallToUpdateVisualIfHoveredOrSelected();

	// Movement Path Generation
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void GenerateMovementPath(const TArray<FIntPoint>& PathIndices);

	// Simple Look-At System (called by abilities)
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void EnableLookAt();
	
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void DisableLookAt();
	
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void SetLookAtLocation(FVector TargetLocation);

	// Damage Numbers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> DamageNumberComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UDamageNumberWidget> DamageNumberWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowDamageNumber(float DamageAmount);

private:
	// Look-at variables
	UPROPERTY()
	bool bShouldLookAt = false;
	
	UPROPERTY()
	FRotator OriginalRotation = FRotator::ZeroRotator;
	
	UPROPERTY()
	FVector LookAtTargetLocation = FVector::ZeroVector;
protected:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float LookAtSpeed = 5.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
