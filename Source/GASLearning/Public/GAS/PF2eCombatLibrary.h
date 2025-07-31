#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PF2eCombatLibrary.generated.h"

UENUM(BlueprintType)
enum class EDegreeOfSuccess : uint8
{
	CriticalFailure		UMETA(DisplayName = "Critical Failure"),
	Failure				UMETA(DisplayName = "Failure"),
	Success				UMETA(DisplayName = "Success"),
	CriticalSuccess		UMETA(DisplayName = "Critical Success")
};

UCLASS()
class GASLEARNING_API UPF2eCombatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Core PF2e degree of success calculation
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PF2e Combat")
	static EDegreeOfSuccess CalculateDegreeOfSuccess(int32 Roll, int32 DC, int32 D20Result);

	// Specific roll functions
	UFUNCTION(BlueprintCallable, Category = "PF2e Combat")
	static EDegreeOfSuccess RollAttack(int32 AttackBonus, int32 TargetAC, int32 MaxDieRoll = 20);

	UFUNCTION(BlueprintCallable, Category = "PF2e Combat")
	static EDegreeOfSuccess RollSave(int32 SaveBonus, int32 DC, int32 MaxDieRoll = 20);

	UFUNCTION(BlueprintCallable, Category = "PF2e Combat")
	static EDegreeOfSuccess RollSkillCheck(int32 SkillBonus, int32 DC, int32 MaxDieRoll = 20);

	// Range penalty version for ranged attacks
	UFUNCTION(BlueprintCallable, Category = "PF2e Combat")
	static EDegreeOfSuccess RollAttackWithPenalty(int32 AttackBonus, int32 TargetAC, int32 RangePenalty, int32 MaxDieRoll = 20);

	// Multiple Attack Penalty calculation
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PF2e Combat")
	static float CalculateMAPPenalty(class UAbilitySystemComponent* ASC, bool bIsAgile);

	// Damage calculation helpers
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PF2e Combat")
	static float ApplyDamageMultiplier(float BaseDamage, EDegreeOfSuccess Result);

	// General damage rolling function
	UFUNCTION(BlueprintCallable, Category = "PF2e Combat")
	static float RollDamage(float DamageDie, float DamageDieCount, float DamageBonus = 0.0f);

	// Difficulty control - MaxDieRoll for controlling crits
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PF2e Combat")
	static int32 RollD20(int32 MaxDieRoll = 20);

	// Utility functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PF2e Combat")
	static FString GetDegreeOfSuccessString(EDegreeOfSuccess Degree);

private:
	// Internal helper for the natural 20/1 logic
	static EDegreeOfSuccess ApplyNaturalRollModifiers(EDegreeOfSuccess BaseResult, int32 D20Result);
};