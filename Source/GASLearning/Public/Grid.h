#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Actor.h>
#include "Components/StaticMeshComponent.h"
#include "StructsAndEnums/F_TileData.h"
#include "StructsAndEnums/E_TileType.h"
#include "StructsAndEnums/E_TileState.h"
#include "StructsAndEnums/FS_IntPointArray.h"
#include "Grid.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API AGrid : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
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

};
