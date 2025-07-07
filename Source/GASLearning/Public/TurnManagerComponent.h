#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StructsAndEnums/FS_IntPointArray.h"
#include "TurnManagerComponent.generated.h"
class ACombatant;
class URangeFinder;
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class GASLEARNING_API UTurnManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTurnManagerComponent();
	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	TArray<TObjectPtr<ACombatant>> CombatantArray;//holds the array of all combatants in fight before sorting them in initiative, and after sorting.

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	TMap<float, TObjectPtr<ACombatant>> CurrentInitiativeHolder; //Holds the rolls for initiative 

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	bool bIsCombatActive = false; //This is generally useless safety check.

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	int32 CurrentTurnActions = 0; //Number of actions combatant has this turn, we get if from CurrentCombatant->BeginTurn()

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	int32 CombatantTurn = 0; //The member of Combatant Array whose turn it is.

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	int32 UnitToRemoveIndex = -1; //This is just holder for NPC death in array form
 
	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	TObjectPtr<ACombatant> UnitToRemove = nullptr; //This is just holder for NPC death in pointer form (yeah, it can be redundant, but I still need it)

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	TObjectPtr<ACombatant> CurrentCombatant = nullptr;//pointer at the combatant who's the owner of Combatant Turn in Combatant Array (yes, it's overcomplicated for no reason)

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	FS_IntPointArray EffectRangeTEMP; //Temporary storage for effect ranges, this is used for grid mesh highlights

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	FS_IntPointArray EffectAreaTEMP; //Temporary storage for effect areas, this is used for grid mesh instance highlights

	// Helper to get owning Grid
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	class AGrid* GetOwningGrid() const;

	// Start combat and sort by initiative
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartCombat();
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void SpendActions(int32 ActionsToSpend);
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void EndTurn();
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void EndCombat();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitializeCombat(); //initialise combat
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RemoveCombatant(ACombatant* CombatantToRemove);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void IncapacitateCombatant(ACombatant* CombatantToRemove);
//Spells
	UFUNCTION(BlueprintCallable, Category = "Combat")
	TArray<FIntPoint> GetEffectRange(uint8 Range, bool IgnoreLOS, bool IgnoreOrigin);
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	TArray<FIntPoint> GetSpellArea(FIntPoint Origin, FIntPoint CasterLocation);
//Unit spawning
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetUnitOnGrid(ACombatant* Combatant, FIntPoint Index, bool Force);
	// Blueprint implementable event for enemy spawning
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void SpawnEnemies();
};