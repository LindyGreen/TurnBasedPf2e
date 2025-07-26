#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Actor.h>
#include "Components/StaticMeshComponent.h"
#include "StructsAndEnums/F_TileData.h"
#include "StructsAndEnums/E_TileType.h"
#include "StructsAndEnums/E_TileState.h"
#include "StructsAndEnums/FS_IntPointArray.h"
#include "Grid.generated.h"

class UGridMeshInstance;
class UMovementSplineComponent;

UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API AGrid : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGrid();


	
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	TObjectPtr<UStaticMeshComponent> MeshHandle;

	// Grid Variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid")
	FVector GridStartingPosition;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid")
	FVector FinalTileSize;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GridTileCount")
	FVector TileSize;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GridTileCount")
	double GridOffsetFromGround = 2.0;
	
	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	TMap<FIntPoint, FS_TileData> GridTiles;
	
	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	bool bIsHeightFound;
	
	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	TMap<ETileState, FS_IntPointArray> TileStatesToIndexes;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	UGridMeshInstance* GridMeshInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<UMovementSplineComponent> MovementSpline;

	// Functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	bool IsIndexValid(FIntPoint Index) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TileIndexing")
	FVector GetTileLocationFromIndex(FIntPoint GridIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	TArray<FIntPoint> GetAllTilesWithState(ETileState State);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	FVector CalculateSquaresSpawnLocation(FIntPoint GridIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void CalculateStartingPosition();

	UFUNCTION(BlueprintCallable, Category = "Tile Indexing")
	FVector TraceForGround(FVector PotentialLocation, ETileType& OutTileType);

	UFUNCTION(BlueprintCallable, Category = "Tile Indexing")
	FVector GetCursorLocationOnGrid();

	UFUNCTION(BlueprintCallable, Category = "Tile Indexing")
	FIntPoint GetIndexFromWorldLocation(FVector InputLocation);
 	UFUNCTION(BlueprintCallable, Category = "Tile Indexing")
	FIntPoint GetTileIndexUnderCursor();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	FS_IntPointArray RemoveObstacleTiles(const FS_IntPointArray& InputArray);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void AddStateToTile(FIntPoint TileIndex, ETileState State);
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void RemoveStateFromTile(FIntPoint TileIndex, ETileState State);
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void ClearStateFromTiles(ETileState State);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void GenerateMovementPath(const FVector& StartLocation, const TArray<FIntPoint>& PathIndices, float CapsuleHalfHeight = 50.0f);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	FORCEINLINE AActor* GetCombatantUnderIndex(FIntPoint Index) const
	{
		if (const FS_TileData* TileData = GridTiles.Find(Index))
		{
			return TileData->UnitOnTile.Get();
		}
		return nullptr;
	}
	
};
