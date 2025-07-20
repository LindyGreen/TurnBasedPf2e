#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Combatant.h"
#include "InitiativeTrackerWidget.generated.h"

class UTurnManagerComponent;
class UInitiativeEntryWidget;
class UAbilityWidgetEntry;

UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API UInitiativeTrackerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UInitiativeTrackerWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// Widget Components - should be a Vertical Box in Blueprint
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UVerticalBox> InitiativeList;

	// SpendAction button
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UButton> SpendActionButton;

	// Ability hotbar container
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> AbilityHotbar;

	// Reference to Turn Manager
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<UTurnManagerComponent> TurnManagerRef;

	// Entry widget class to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UInitiativeEntryWidget> InitiativeEntryClass;

	// Ability widget class to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UAbilityWidgetEntry> AbilityWidgetClass;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitializeTracker(UTurnManagerComponent* InTurnManager);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UpdateInitiativeOrder(const TArray<ACombatant*>& SortedCombatants);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UpdateCurrentTurn(ACombatant* CurrentCombatant);


	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ClearTracker();

	// Delegate handlers
	UFUNCTION()
	void OnCombatStarted(const TArray<ACombatant*>& SortedCombatants);

	UFUNCTION()
	void OnTurnChanged(ACombatant* NewCurrentCombatant);

	UFUNCTION()
	void OnCombatEnded();


private:
	// Store entry widgets for updates
	UPROPERTY()
	TArray<TObjectPtr<UInitiativeEntryWidget>> EntryWidgets;

	// Helper functions
	void CreateEntryWidget(ACombatant* Combatant, int32 Index);
	UInitiativeEntryWidget* FindEntryForCombatant(ACombatant* Combatant);
	void CreateAbilityWidgetEntry(ACombatant* Combatant);
};