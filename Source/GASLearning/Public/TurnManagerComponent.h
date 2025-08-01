#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StructsAndEnums/FS_IntPointArray.h"
#include "TurnManagerComponent.generated.h"
class ACombatant;
class URangeFinder;
class AGrid;
// Delegates for UI communication
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStarted, const TArray<ACombatant*>&, SortedCombatants);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, ACombatant*, NewCurrentCombatant);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatEnded);
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class GASLEARNING_API UTurnManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTurnManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	TArray<TObjectPtr<ACombatant>> CombatantArray;//holds the array of all combatants in fight before sorting them in initiative, and after sorting.

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	TMap<float, TObjectPtr<ACombatant>> CurrentInitiativeHolder; //Holds the rolls for initiative 

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	bool bIsCombatActive = false; //This is generally useless safety check.
	
	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	int32 CombatantTurn = 0; //The member of Combatant Array whose turn it is.

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	int32 UnitToRemoveIndex = -1; //This is just holder for NPC death in array form
 
	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	TObjectPtr<ACombatant> UnitToRemove = nullptr; //This is just holder for NPC death in pointer form (yeah, it can be redundant, but I still need it)

	UPROPERTY(BlueprintReadWrite, Category = "Turn Manager")
	TObjectPtr<ACombatant> CurrentCombatant = nullptr;//pointer at the combatant who's the owner of Combatant Turn in Combatant Array (yes, it's overcomplicated for no reason)

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	TObjectPtr<AGrid> GridReference;


	// UI Delegates
	UPROPERTY(BlueprintAssignable, Category = "Combat Events")
	FOnCombatStarted OnCombatStarted;

	UPROPERTY(BlueprintAssignable, Category = "Combat Events")
	FOnTurnChanged OnTurnChanged;


	UPROPERTY(BlueprintAssignable, Category = "Combat Events")
	FOnCombatEnded OnCombatEnded;
	
	// Start combat and sort by initiative
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartCombat();
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void OnActionSpentInCombatant(float ActionsLeft);
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
//Unit spawning
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetUnitOnGrid(ACombatant* Combatant, FIntPoint Index, bool Force);
	// Blueprint implementable event for enemy spawning
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void SpawnEnemies(const FName& EnemiesOnMapRowName);
};