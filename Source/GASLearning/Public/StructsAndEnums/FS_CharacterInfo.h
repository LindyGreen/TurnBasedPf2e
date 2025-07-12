#pragma once

#include "CoreMinimal.h"
#include "FS_CharacterInfo.generated.h"

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_CharacterInfo
{
	GENERATED_BODY()

public:
	// Character identification
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Character Name"))
	FText CharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Character Description"))
	FText CharacterDescription;

	// Character type/archetype (for differentiation)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Character Type"))
	FString CharacterType;

	// Character progression
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Character Level"))
	int32 Level;

	FS_CharacterInfo()
	{
		CharacterName = FText::FromString("Character");
		CharacterDescription = FText::FromString("Default Description");
		CharacterType = "Fighter";
		Level = 1;
	}
};