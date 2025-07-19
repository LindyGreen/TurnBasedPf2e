// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RangeFinderLibrary.h"
#include "StructsAndEnums/E_TileType.h"
#include "RangeFinder.generated.h"

class AGrid;

UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class GASLEARNING_API URangeFinder : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URangeFinder();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	//Functions and Variables I need.
	/*1. Origin Intpoint
	 *2. Target Intpoint
	 *3. Intpoint Array for a shape
	 *4. range
	 *5. Pattern Enum
	 */
UPROPERTY()
	TArray<FIntPoint> PossibleArray;
	//Functions
#pragma region Functions 
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GeneratePossibleArray(FIntPoint OriginPoint, FIntPoint CasterLocation, uint8 Area,
	                                        EAE_SpellPattern Enum, bool bIgnoreOrigin);
	UFUNCTION(BlueprintCallable) //Emanation is callable because it is used in ability ranges.
	TArray<FIntPoint> GenerateEmanation(FIntPoint OriginPoint, uint8 Area, bool bIgnoreOrigin);

	TArray<FIntPoint> GenerateCone(FIntPoint OriginPoint, FIntPoint CasterLocation, uint8 Area);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 TotalCost(FIntPoint OriginPoint, FIntPoint CurrentTile);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FIntPoint> GenerateLine(FIntPoint CasterLocation, FIntPoint OriginPoint, uint8 Area);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FIntPoint> GenerateBurst(FIntPoint OriginPoint, uint8 Area);

	UFUNCTION(BlueprintCallable) 
	void RemoveInvalidNeighbors(FIntPoint CurrentArrayElement, FIntPoint Index, TArray<FIntPoint>& DiagonalsArray); 

	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GetNeighborIndexes(FIntPoint Index, bool bIncludeDiagonals, TArray<FIntPoint>& Diagonals);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsDiagonal(FIntPoint Index1, FIntPoint Index2);
	UFUNCTION(BlueprintCallable)
	void InsertTileInDiscoveredArray(FS_PathfindingData CurrentTileData);
	UFUNCTION(BlueprintCallable)
	void DiscoverTile(FS_PathfindingData TilePathData);
	UFUNCTION(BlueprintCallable)
	FS_PathfindingData PullCheapestTileOutOfDiscoveredList();
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GeneratePath();
	UFUNCTION(BlueprintCallable)
	bool DiscoverNextNeighbor();
	UFUNCTION(BlueprintCallable)
	bool LoopThroughNeighbors();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsTileTypeWalkable(ETileType TileType);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsTileWalkable(FIntPoint Index);
	
	UFUNCTION(BlueprintCallable)
	void CleanGeneratedData();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInputDataValid();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FS_PathfindingData> GetValidTileNeighbors(FIntPoint Index, TArray<ETileType> ValidTypes);
#pragma endregion Functions
#pragma region input variables
	UPROPERTY(BlueprintReadWrite, Category="Input")
	FIntPoint Start;
	UPROPERTY(BlueprintReadWrite, Category="Input")
	bool Reachable;
	UPROPERTY(BlueprintReadWrite, Category="Input")
	int32 MaxPathLength;
#pragma endregion	
#pragma region Tiles
	UPROPERTY(BlueprintReadWrite, Category="Tiles")
	TArray<FIntPoint> DiscoveredTileIndexes;
	UPROPERTY(BlueprintReadWrite, Category="Tiles")
	TArray<int32> DiscoveredTileSortedCost;
	UPROPERTY(BlueprintReadWrite, Category="Tiles")
	TArray<FIntPoint> AnalizedTileIndexes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tiles")
	FIntPoint Origin;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tiles")  
	FIntPoint Target;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tiles")
	FS_PathfindingData CurrentDiscoveredTile;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tiles")
	FS_PathfindingData CurrentNeighbor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tiles")
	TArray<FS_PathfindingData> CurrentNeighbors;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tiles")
	TMap<FIntPoint, FS_PathfindingData> PathfindingData;
//	UPROPERTY(BlueprintReadWrite, Category="Tiles")
	
#pragma endregion 

#pragma region Grid Reference
	UPROPERTY(BlueprintReadWrite, Category="Grid")
	TObjectPtr<AGrid> GridReference;
#pragma endregion 

};
