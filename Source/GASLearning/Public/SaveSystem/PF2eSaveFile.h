#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "StructsAndEnums/FS_CompleteCharacterData.h"
#include "PF2eSaveFile.generated.h"

UCLASS(BlueprintType)
class GASLEARNING_API UPF2eSaveFile : public USaveGame
{
	GENERATED_BODY()

public:
	UPF2eSaveFile();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	FS_CompleteCharacterData Chad;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	FS_CompleteCharacterData Rogue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	FS_CompleteCharacterData Archer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	FS_CompleteCharacterData Sorcerer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	FDateTime SaveDate;
};