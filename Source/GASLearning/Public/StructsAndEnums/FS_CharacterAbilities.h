#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "FS_CharacterAbilities.generated.h"

class UMyBaseGameplayAbility;

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_CharacterAbilities
{
	GENERATED_BODY()

public:
	// Abilities this character starts with
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Starting Abilities"))
	TArray<TSubclassOf<UMyBaseGameplayAbility>> StartingAbilities;

	// GameplayEffects to apply on spawn (for buffs, conditions, etc)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Starting Effects"))
	TArray<TSubclassOf<UGameplayEffect>> StartingEffects;

	// Abilities that can be learned/unlocked later
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Available Abilities"))
	TArray<TSubclassOf<UMyBaseGameplayAbility>> AvailableAbilities;

	// Additional gameplay tags for character traits, conditions, etc.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Character Traits"))
	FGameplayTagContainer CharacterTraits;

	FS_CharacterAbilities()
	{
		StartingAbilities = TArray<TSubclassOf<UMyBaseGameplayAbility>>();
		StartingEffects = TArray<TSubclassOf<UGameplayEffect>>();
		AvailableAbilities = TArray<TSubclassOf<UMyBaseGameplayAbility>>();
		CharacterTraits = FGameplayTagContainer();
	}
};