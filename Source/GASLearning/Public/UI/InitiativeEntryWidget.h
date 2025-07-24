#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Combatant.h"
#include "InitiativeEntryWidget.generated.h"
class UTextBlock;
class UProgressBar;
class UBorder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionsChangedForAbilityRefresh);

UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API UInitiativeEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UInitiativeEntryWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	// Widget Components
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CharacterNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> InitiativeValueText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ActionsRemainingText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UBorder> EntryBorder;

	// Data
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TObjectPtr<ACombatant> CombatantRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor CurrentTurnColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor NormalColor = FLinearColor::White;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetupEntry(ACombatant* InCombatant, float InitiativeValue);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UpdateActionsDisplay(int32 ActionsRemaining, int32 MaxActions);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UpdateHealthDisplay();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCurrentTurn(bool bIsCurrentTurn);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UpdateReactionDisplay(bool bHasReaction);

	// Delegate to notify when actions change (for ability button refresh)
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnActionsChangedForAbilityRefresh OnActionsChangedForAbilityRefresh;

	// Delegate handlers for real-time updates
	UFUNCTION()
	void OnHealthChanged(float Magnitude, float NewHealth);
	
	UFUNCTION()
	void OnActionsChanged(float Magnitude, float NewActionsRemaining);

private:
	void UpdateDisplay();
};