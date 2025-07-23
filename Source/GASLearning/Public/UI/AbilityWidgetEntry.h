#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "StructsAndEnums/EAbilityVariantType.h"
#include "AbilityWidgetEntry.generated.h"
struct FGameplayAbilitySpec;
class UMyBaseGameplayAbility;
class UAbilitySystemComponent;

UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API UAbilityWidgetEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	// Ability reference - supports both C++ and Blueprint abilities
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UMyBaseGameplayAbility> AbilityReference;

	// ASC reference - set in constructor
	UPROPERTY(BlueprintReadOnly, Category = "Ability", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UAbilitySystemComponent> OwnerASC;

	// Cached ability spec for performance
	FGameplayAbilitySpec* CachedAbilitySpec;

	// UI Component references - these are bound when 
	UPROPERTY(BlueprintReadWrite, Category = "UI Components", meta = (BindWidget))
	TObjectPtr<UButton> AbilityButton;

	UPROPERTY(BlueprintReadWrite, Category = "UI Components", meta = (BindWidget))
	TObjectPtr<UImage> AbilityIcon;

	UPROPERTY(BlueprintReadWrite, Category = "UI Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> AbilityName;

	UPROPERTY(BlueprintReadWrite, Category = "UI Components", meta = (BindWidget))
	TObjectPtr<UImage> ActionCost;

	// Setup function to initialize widget with ability data
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void SetupAbilityWidget(UMyBaseGameplayAbility* InAbility);

	// Main click handler - either activates ability or shows variants
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnAbilityClicked();

	// Variant selection handler
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnVariantSelected(EAbilityVariantType Variant);

	// Virtual function for Blueprint override - creates the actual widget UI
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void CreateEntryWidget();

	// Blueprint implementable events for UI updates
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetupAbilityDisplay();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowVariantSelection();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void HideVariantSelection();

protected:
	// Helper function to activate the ability
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ActivateAbility();


	//  Function to enable/disable button based on affordability
	UFUNCTION(BlueprintCallable, Category = "UI")
	void EnableDisableAbilityButton();

	
};