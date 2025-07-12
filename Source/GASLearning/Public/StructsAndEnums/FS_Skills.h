#pragma once

#include "CoreMinimal.h"
#include "FS_Skills.generated.h"

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_Skills
{
	GENERATED_BODY()

public:
	// Core PF2e skills
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Acrobatics"))
	int32 Acrobatics;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Arcana"))
	int32 Arcana;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Athletics"))
	int32 Athletics;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Crafting"))
	int32 Crafting;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Deception"))
	int32 Deception;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Diplomacy"))
	int32 Diplomacy;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Intimidation"))
	int32 Intimidation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Medicine"))
	int32 Medicine;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Nature"))
	int32 Nature;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Occultism"))
	int32 Occultism;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Performance"))
	int32 Performance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Religion"))
	int32 Religion;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Society"))
	int32 Society;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Stealth"))
	int32 Stealth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Survival"))
	int32 Survival;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Thievery"))
	int32 Thievery;

	FS_Skills()
	{
		// Initialize all skills to 0 (untrained, no stat bonus, no level bonus, these will be modifiers i guess)
		Acrobatics = 0;
		Arcana = 0;
		Athletics = 0;
		Crafting = 0;
		Deception = 0;
		Diplomacy = 0;
		Intimidation = 0;
		Medicine = 0;
		Nature = 0;
		Occultism = 0;
		Performance = 0;
		Religion = 0;
		Society = 0;
		Stealth = 0;
		Survival = 0;
		Thievery = 0;
	}
};