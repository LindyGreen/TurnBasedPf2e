#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FS_CharacterInfo.h"
#include "FS_CombatantStats.h" 
#include "FS_CombatantDataAssets.h"
#include "FS_SpellResources.h"
#include "FS_Skills.h"
#include "FS_CompleteCharacterData.generated.h"

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_CompleteCharacterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Basic character identity
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Character Info"))
	FS_CharacterInfo CharacterInfo;
	
	// Combat-related stats and abilities  (abilities include spells, but wont work without spell resources)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Combat Stats"))
	FS_CombatantStats CombatantStats;

	// Visual assets and animations
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Visual Assets"))
	FS_CombatantDataAssets VisualAssets;

	// Spellcasting resources and attributes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Spell Resources"))
	FS_SpellResourcesAndAttributes SpellResources;

	// Skill proficiencies
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Skills"))
	FS_Skills Skills;

	FS_CompleteCharacterData()
	{
		CharacterInfo = FS_CharacterInfo();
		CombatantStats = FS_CombatantStats();
		VisualAssets = FS_CombatantDataAssets();
		SpellResources = FS_SpellResourcesAndAttributes();
		Skills = FS_Skills();
	}
};