#pragma once

#include "CoreMinimal.h"
#include "GAS/MyBaseGameplayAbility.h"
#include "GAS/PF2eCombatLibrary.h"
#include "BasicDamageSpellAbility.generated.h"

UENUM(BlueprintType)
enum class ESpellArea : uint8
{
	Single			UMETA(DisplayName = "Single Target"),
	Line			UMETA(DisplayName = "Line"),
	Cone			UMETA(DisplayName = "Cone"),
	Burst			UMETA(DisplayName = "Burst"),
	Emanation		UMETA(DisplayName = "Emanation")
};

UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API UBasicDamageSpellAbility : public UMyBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UBasicDamageSpellAbility();

	// Spell properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 SpellLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	ESpellArea AreaType = ESpellArea::Single;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 AreaSize = 1; // Radius for burst/emanation, length for line/cone

	// Damage properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 BaseDamage = 6; // Base damage die

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 DamageDiceCount = 2; // Number of dice (e.g., 2d6)

	// Saving throw properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	bool bAllowsSave = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	int32 SaveDC = 15; // Spell save DC

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Helper functions
	UFUNCTION(BlueprintCallable, Category = "Spell")
	int32 RollDamage() const;

	UFUNCTION(BlueprintCallable, Category = "Spell")
	EDegreeOfSuccess RollSavingThrow(int32 TargetSaveBonus = 5) const;

	UFUNCTION(BlueprintCallable, Category = "Spell")
	TArray<FIntPoint> GetSpellArea(FIntPoint TargetLocation) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Spell")
	void OnSpellCriticalSuccess(class ACombatant* Target);

	UFUNCTION(BlueprintImplementableEvent, Category = "Spell")
	void OnSpellSuccess(class ACombatant* Target, int32 HalfDamage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Spell")
	void OnSpellFailure(class ACombatant* Target, int32 Damage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Spell")
	void OnSpellCriticalFailure(class ACombatant* Target, int32 DoubleDamage);
};