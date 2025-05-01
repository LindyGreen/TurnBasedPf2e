// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RangeFinderLibrary.h"
#include "RangeFinder.generated.h"

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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
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
	void RemoveInvalidNeighbors(FIntPoint CurrentArrayElement, TArray<FIntPoint>& NeighborsToRemove, FIntPoint Index); 
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GetNeighborIndexes(FIntPoint Index, bool bIncludeDiagonals, TArray<FIntPoint>& Diagonals);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsDiagonal(FIntPoint Index1, FIntPoint Index2);
#pragma endregion Functions
#pragma region input variables
	UPROPERTY(BlueprintReadWrite, Category="Input")
	FIntPoint Start;
	UPROPERTY(BlueprintReadWrite, Category="Input")
	bool Reachable;
	UPROPERTY(BlueprintReadWrite, Category="Input")
	int32 MaxPathLength;
	//Structures
	
	
#pragma endregion	
#pragma region Tiles
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
};
