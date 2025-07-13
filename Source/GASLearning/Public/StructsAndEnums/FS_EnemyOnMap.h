#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FS_CompleteCharacterData.h"
#include "StateTree.h"
#include "FS_EnemyOnMap.generated.h"

UENUM(BlueprintType)
enum class EFacingDirection : uint8
{
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"), 
	Up  		UMETA(DisplayName = "Up"),
	Down		UMETA(DisplayName = "Down")
};

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_EnemyWithAI
{
	GENERATED_BODY()

public:
	// AI behavior for this enemy
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="AI State Tree"))
	TSoftObjectPtr<UStateTree> AIBehavior;

	// Where to spawn on the grid
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Spawn Location"))
	FIntPoint SpawnLocation;

	// Which direction the enemy faces initially
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Facing Direction"))
	EFacingDirection FacingDirection;

	// Complete character data for this enemy
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Character Data"))
	FS_CompleteCharacterData CharacterData;

	FS_EnemyWithAI()
	{
		AIBehavior = nullptr;
		SpawnLocation = FIntPoint(0, 0);
		FacingDirection = EFacingDirection::Left;
		CharacterData = FS_CompleteCharacterData();
	}
};

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_EnemiesOnMap : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Array of enemies to spawn for this encounter
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Enemies"))
	TArray<FS_EnemyWithAI> Enemies;

	FS_EnemiesOnMap()
	{
		Enemies = TArray<FS_EnemyWithAI>();
	}
};