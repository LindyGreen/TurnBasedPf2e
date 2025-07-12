#pragma once

#include "CoreMinimal.h"
#include "FS_SpellResources.generated.h"

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_SpellResourcesAndAttributes
{
	GENERATED_BODY()

public:
	// Casting type
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Is Spontaneous Caster"))
	bool bIsSpontaneous;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Cantrips You can prepare"))
	int32 PreperableCantrips;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 1 Slots"))
	int32 Level1Slots;//for spontaneous casters
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 1 Slot1"))
	bool Level1Slot1;//For prepared casters
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 1 Slot2"))
	bool Level1Slot2;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 1 Slot3"))
	bool Level1Slot3;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 2 Slots"))
	int32 Level2Slots;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 2 Slot1"))
	bool Level2Slot1;//For prepared casters
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 2 Slot2"))
	bool Level2Slot2;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 2 Slot3"))
	bool Level2Slot3;
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 3 Slots"))
	int32 Level3Slots;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 3 Slot1"))
    	bool Level3Slot1;//For prepared casters
    	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 3 Slot2"))
    	bool Level3Slot2;
    	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Level 3 Slot3"))
    	bool Level3Slot3;
	
	// Divine font (For Chad the protagonist)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Max DivineFont"))
	int32 DivineFont;
    
	// Focus Points (PF2e mechanic)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Max Focus Points"))
	int32 MaxFocusPoints;

	// Spell attack and DC
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Spell Attack Bonus"))
	int32 SpellAttackBonus;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Spell Save DC"))
	int32 SpellSaveDC;

	FS_SpellResourcesAndAttributes()
	{
		bIsSpontaneous = false; // Default to prepared caster
		PreperableCantrips = 0;
		Level1Slots = 0;
		Level1Slot1 = false;
		Level1Slot2 = false;
		Level1Slot3 = false;
		Level2Slots = 0;
		Level2Slot1 = false;
		Level2Slot2 = false;
		Level2Slot3 = false;
		Level3Slots = 0;
		Level3Slot1 = false;
		Level3Slot2 = false;
		Level3Slot3 = false;
		DivineFont = 0;
		MaxFocusPoints = 0;
		SpellAttackBonus = 0;
		SpellSaveDC = 10;
	}
};