// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeFinder.h"
#include "Algo/Reverse.h"
#include "LogTypes.h"
#include "Grid.h"
// Sets default values for this component's properties
URangeFinder::URangeFinder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void URangeFinder::BeginPlay()
{
	Super::BeginPlay();

	// Get Grid reference from owner
	if (AActor* Owner = GetOwner())
	{
		GridReference = Cast<AGrid>(Owner);
		if (!GridReference)
		{
			UE_LOG(LogTemp, Warning,
			       TEXT("URangeFinder::BeginPlay - Owner is not a Grid actor"));
		}
	}
}


TArray<FIntPoint> URangeFinder::GeneratePossibleArray(
	FIntPoint OriginPoint, FIntPoint CasterLocation, uint8 Area,
	EAE_SpellPattern Enum, bool bIgnoreOrigin)
{
	switch (Enum)
	{
	case EAE_SpellPattern_Invalid:
		UE_LOG(Log_Grid, Display,
		       TEXT("URangeFinder::GeneratePossibleArray: Invalid Enum"));
		break;
	case EAE_SpellPattern_Burst:
		return TArray<FIntPoint>(GenerateBurst(OriginPoint, Area));
	case EAE_SPellPattern_Line:
		return TArray<FIntPoint>(
			GenerateLine(CasterLocation, OriginPoint, Area));
	case EAE_SpellPattern_Cone:
		return TArray<FIntPoint>(
			GenerateCone(OriginPoint, CasterLocation, Area));
	case EAE_SpellPattern_Emanation:
		return TArray<FIntPoint>(
			GenerateEmanation(OriginPoint, Area, bIgnoreOrigin));
		//SHOULD CHANGE TO CASTER LOCATION In DEV
	}
	return TArray<FIntPoint>();
}

TArray<FIntPoint> URangeFinder::GenerateEmanation(
	FIntPoint OriginPoint, uint8 Area, bool bIgnoreOrigin)
{
	TArray<FIntPoint> ValidTiles;
	for (int32 dx = -Area; dx <= Area; dx++)
	{
		for (int32 dy = -Area; dy <= Area; dy++)
		{
			FIntPoint CurrentTile(OriginPoint.X + dx, OriginPoint.Y + dy);
			if (TotalCost(OriginPoint, CurrentTile) <= Area)
			{
				ValidTiles.Add(CurrentTile);
			}
		}
	}
	if (bIgnoreOrigin)
		ValidTiles.Remove(OriginPoint);
	return ValidTiles;
}

TArray<FIntPoint> URangeFinder::GenerateCone(FIntPoint OriginPoint,
                                             FIntPoint CasterLocation,
                                             uint8 Area)
{
	int32 ConeRange = Area - 1; //to count in origin point.
	TArray<FIntPoint> ValidTiles; //init return
	FIntPoint RelativePosition = OriginPoint - CasterLocation;
	//init RelativeToCaster
	bool bIsOrthogonal = abs(RelativePosition.X + RelativePosition.Y) == 1;
	if (bIsOrthogonal)
	{
		//SETUP THE DIRECTION TO SHOOT AT// this will set up cone direction.
		int32 startX, endX, startY, endY;

		if (RelativePosition.X == 1) // Shooting top
		{
			startX = 0;
			endX = ConeRange;
			startY = -ConeRange;
			endY = ConeRange;
		}
		else if (RelativePosition.X == -1) // Shooting Down
		{
			startX = -ConeRange;
			endX = 0;
			startY = -ConeRange;
			endY = ConeRange;
		}
		else if (RelativePosition.Y == 1) // Shooting Left
		{
			startX = -ConeRange;
			endX = ConeRange;
			startY = 0;
			endY = ConeRange;
		}
		else // Shooting down
		{
			startX = -ConeRange;
			endX = ConeRange;
			startY = -ConeRange;
			endY = 0;
		}

		for (int32 dx = startX; dx <= endX; dx++) //removing tiles too far away

		{
			for (int32 dy = startY; dy <= endY; dy++)
			{
				FIntPoint CurrentTile(OriginPoint.X + dx, OriginPoint.Y + dy);
				FIntPoint RelativeTile(CurrentTile - OriginPoint);
				if ((TotalCost(CasterLocation, CurrentTile) <= Area) && (
					(RelativePosition.X == 0)
						? abs(RelativeTile.X) <= abs(RelativeTile.Y)
						: abs(RelativeTile.Y) <= abs(RelativeTile.X)))
				{
					ValidTiles.Add(CurrentTile);
				}
			}
		}
		UE_LOG(Log_Grid, Log,
		       TEXT("URangeFinder::GenerateCone - OriginPoint is orthogonal"));
	}
	else //diagonals
	{
		int32 startX = (RelativePosition.X > 0) ? 0 : -ConeRange;
		int32 endX = (RelativePosition.X > 0) ? ConeRange : 0;
		int32 startY = (RelativePosition.Y > 0) ? 0 : -ConeRange;
		int32 endY = (RelativePosition.Y > 0) ? ConeRange : 0;
		for (int32 dx = startX; dx <= endX; dx++)

		{
			for (int32 dy = startY; dy <= endY; dy++)
			{
				FIntPoint CurrentTile(OriginPoint.X + dx, OriginPoint.Y + dy);
				if (TotalCost(CasterLocation, CurrentTile) <= Area)
				{
					ValidTiles.Add(CurrentTile);
				}
			}
		}
	}
	return ValidTiles;
}

int32 URangeFinder::TotalCost(FIntPoint OriginPoint, FIntPoint CurrentTile)
//Min Distance Calculation
{
	return FMath::Max(abs(OriginPoint.X - CurrentTile.X),
	                  abs(OriginPoint.Y - CurrentTile.Y)) +
		FMath::Min(abs(OriginPoint.X - CurrentTile.X),
		           abs(OriginPoint.Y - CurrentTile.Y)) / 2;
} //Min Cost calc
TArray<FIntPoint> URangeFinder::GenerateLine(FIntPoint CasterLocation,
                                             FIntPoint OriginPoint, uint8 Area)
{
	TArray<FIntPoint> ValidTiles;
	if (TotalCost(CasterLocation, OriginPoint) > Area)
	//return empty array of total cost>range
	{
		return ValidTiles;
	}
	int32 dx = abs(OriginPoint.X - CasterLocation.X);
	int32 dy = abs(OriginPoint.Y - CasterLocation.Y);
	int32 x = CasterLocation.X;
	int32 y = CasterLocation.Y;

	int32 stepX = (CasterLocation.X < OriginPoint.X) ? 1 : -1; //Up or Down
	int32 stepY = (CasterLocation.Y < OriginPoint.Y) ? 1 : -1; //LeftOrRight

	int32 err = dx - dy;

	while (x != OriginPoint.X || y != OriginPoint.Y)
	{
		ValidTiles.Add(FIntPoint(x, y));
		int32 e2 = 2 * err;

		if (e2 > -dy)
		{
			err -= dy;
			x += stepX;
		}
		if (e2 < dx)
		{
			err += dx;
			y += stepY;
		}
	}
	ValidTiles.AddUnique(OriginPoint);
	ValidTiles.Remove(CasterLocation);


	return ValidTiles;
}

TArray<FIntPoint> URangeFinder::GenerateBurst(FIntPoint OriginPoint, uint8 Area)
{
	TArray<FIntPoint> BurstArray;
	BurstArray.Append(GenerateCone(OriginPoint, OriginPoint + FIntPoint(1, 1),
	                               Area));
	BurstArray.Append(GenerateCone(OriginPoint + FIntPoint(1, 0),
	                               OriginPoint + FIntPoint(0, 1), Area));
	BurstArray.Append(GenerateCone(OriginPoint + FIntPoint(1, 1),
	                               OriginPoint + FIntPoint(0, 0), Area));
	BurstArray.Append(GenerateCone(OriginPoint + FIntPoint(0, 1),
	                               OriginPoint + FIntPoint(1, 0), Area));
	return BurstArray;
}

void URangeFinder::RemoveInvalidNeighbors(FIntPoint CurrentArrayElement,
                                          FIntPoint Index,
                                          TArray<FIntPoint> &DiagonalsArray)
{
	FIntPoint TempElement = CurrentArrayElement - Index;
	TArray<FIntPoint> NeighborsToRemove;

	if (TempElement.X == -1)
	{
		NeighborsToRemove = {
			FIntPoint(Index.X - 1, Index.Y - 1),
			FIntPoint(Index.X - 1, Index.Y + 1)
		};
	}
	else if (TempElement.X == 1)
	{
		NeighborsToRemove = {
			FIntPoint(Index.X + 1, Index.Y + 1),
			FIntPoint(Index.X + 1, Index.Y - 1)
		};
	}
	else if (TempElement.Y == -1)
	{
		NeighborsToRemove = {
			FIntPoint(Index.X + 1, Index.Y - 1),
			FIntPoint(Index.X - 1, Index.Y - 1)
		};
	}
	else if (TempElement.Y == 1)
	{
		NeighborsToRemove = {
			FIntPoint(Index.X - 1, Index.Y + 1),
			FIntPoint(Index.X + 1, Index.Y + 1)
		};
	}

	// Remove invalid neighbors from diagonals array
	for (const FIntPoint& ToRemove : NeighborsToRemove)
	{
		DiagonalsArray.Remove(ToRemove);
	}
}

TArray<FIntPoint> URangeFinder::GetNeighborIndexes(
	FIntPoint Index, bool bIncludeDiagonals, TArray<FIntPoint>& Diagonals)
{
	TArray<FIntPoint> Array;
	Array = {
		Index + FIntPoint(0, 1),
		Index + FIntPoint(1, 0),
		Index + FIntPoint(0, -1),
		Index + FIntPoint(-1, 0)
	};

	if (bIncludeDiagonals)
	{
		//TArray<FIntPoint>
		Diagonals = {
			Index + FIntPoint(1, -1),
			Index + FIntPoint(1, 1),
			Index + FIntPoint(-1, 1),
			Index + FIntPoint(-1, -1)
		};
		// Array.Append(Diagonals);
	}
	return Array;
}

bool URangeFinder::IsDiagonal(FIntPoint Index1, FIntPoint Index2)
{
	TArray<FIntPoint> Diagonals = {
		Index1 + FIntPoint(1, 1),
		Index1 + FIntPoint(1, -1),
		Index1 + FIntPoint(-1, 1),
		Index1 + FIntPoint(-1, -1)
	};
	return Diagonals.Contains(Index2);
}

void URangeFinder::InsertTileInDiscoveredArray(
	FS_PathfindingData CurrentTileData)
{
	int32 SortingCost = CurrentTileData.CostFromStart + CurrentTileData.
		MinimumCostToTarget;
	if (DiscoveredTileIndexes.IsEmpty())
	{
		//if first just add
		DiscoveredTileSortedCost.Add(SortingCost);
		DiscoveredTileIndexes.Add(CurrentTileData.Index);
	}
	else
	{
		//not first so checking if last index has higher sorting cost
		if (!DiscoveredTileSortedCost.IsEmpty())
		//i have no idea why I put this saveguard here, I added to it, it can't be empty
		{
			if (SortingCost >= DiscoveredTileSortedCost.Last())
			{
				DiscoveredTileSortedCost.Add(SortingCost);
				DiscoveredTileIndexes.Add(CurrentTileData.Index);
				//if faster path - just add 
			}
			else
			{
				if (!DiscoveredTileSortedCost.IsEmpty())
					for (int32 i = 0; i < DiscoveredTileSortedCost.Num(); i++)
					{
						if (SortingCost <= DiscoveredTileSortedCost[i])
						{
							DiscoveredTileSortedCost.Insert(SortingCost, i);
							DiscoveredTileIndexes.Insert(
								CurrentTileData.Index, i);
							break;
							//if slower path - insert to the appropriate position
						}
					}
			}
		}
	}
}

void URangeFinder::DiscoverTile(FS_PathfindingData TilePathData)
{
	PathfindingData.Add(TilePathData.Index, TilePathData);
	InsertTileInDiscoveredArray(TilePathData);
}

FS_PathfindingData URangeFinder::PullCheapestTileOutOfDiscoveredList()
{
	FIntPoint TileIndex = DiscoveredTileIndexes[0];
	if (!DiscoveredTileSortedCost.IsEmpty())
		DiscoveredTileSortedCost.RemoveAt(0);
	if (!AnalizedTileIndexes.IsEmpty())
		DiscoveredTileIndexes.RemoveAt(0);

	AnalizedTileIndexes.Add(TileIndex);
	return PathfindingData.FindRef(TileIndex);
}

TArray<FIntPoint> URangeFinder::GeneratePath()
{
	FIntPoint Current = Target;
	TArray<FIntPoint> Path;
	while (Current != Start)
	{
		Path.Add(Current);
		Current = PathfindingData[Current].PreviousIndex;
	}
	Algo::Reverse(Path);

	return Path;
}

bool URangeFinder::DiscoverNextNeighbor()
{
	CurrentNeighbor = CurrentNeighbors[0];
	if (!CurrentNeighbors.IsEmpty())
		CurrentNeighbors.RemoveAt(0);
	int32 CostFromStart = CurrentDiscoveredTile.CostFromStart + CurrentNeighbor.
		CostToEnterTile;

	if (AnalizedTileIndexes.Contains(CurrentNeighbor.Index))
	{
		if (!Reachable) return false;
		if (CostFromStart > PathfindingData[CurrentNeighbor.Index].
			CostFromStart)
			return false;
	}
	if (CostFromStart > MaxPathLength) return false;
	int32 IndexInDiscovered = DiscoveredTileIndexes.Find(CurrentNeighbor.Index);
	if (IndexInDiscovered != -1)
	{
		CurrentNeighbor = PathfindingData[CurrentNeighbor.Index];
		if (CostFromStart >= CurrentNeighbor.CostFromStart) return false;
		if (CurrentNeighbors.Num() > IndexInDiscovered)
			CurrentNeighbors.
				RemoveAt(IndexInDiscovered);
		if (DiscoveredTileSortedCost.Num() > IndexInDiscovered)
			DiscoveredTileSortedCost.RemoveAt(IndexInDiscovered);
	}
	FS_PathfindingData Temp;
	Temp.Index = CurrentNeighbor.Index;
	Temp.CostToEnterTile = CurrentNeighbor.CostToEnterTile;
	Temp.CostFromStart = CostFromStart;
	Temp.MinimumCostToTarget = TotalCost(CurrentNeighbor.Index, Target);
	Temp.PreviousIndex = CurrentDiscoveredTile.Index;
	DiscoverTile(Temp);
	if (CurrentNeighbor.Index == Target)return true;
	return false;
}

bool URangeFinder::LoopThroughNeighbors()
{
	while (CurrentNeighbors.Num() > 0)
	{
		bool Result = DiscoverNextNeighbor();
		if (Result) return true;
	}
	return false;
}

bool URangeFinder::IsTileTypeWalkable(ETileType TileType)
{
	return TileType != ETileType::None && TileType != ETileType::Obstacle;
}

bool URangeFinder::IsTileWalkable(FIntPoint Index)
{
	if (!GridReference)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("URangeFinder::IsTileWalkable - GridReference is null"));
		return false;
	}

	if (GridReference->GridTiles.Contains(Index))
	{
		FS_TileData TileData = GridReference->GridTiles[Index];
		return IsTileTypeWalkable(TileData.TileType);
	}
	return false;
}

void URangeFinder::CleanGeneratedData()
{
	PathfindingData.Empty();
	DiscoveredTileSortedCost.Empty();
	DiscoveredTileIndexes.Empty();
	AnalizedTileIndexes.Empty();
}

bool URangeFinder::IsInputDataValid()
{
	if (Target == Start)
	{
		return false;
	}

	if (!IsTileWalkable(Start))
	{
		return false;
	}

	if (Reachable)
	{
		return true;
	}

	if (TotalCost(Start, Target) > MaxPathLength)
	{
		return false;
	}

	return IsTileWalkable(Target);
}

TArray<FS_PathfindingData> URangeFinder::GetValidTileNeighbors(
	FIntPoint Index, TArray<ETileType> ValidTypes)
{
	TArray<FS_PathfindingData> Result;
	
	FS_TileData* IndexTileData = GridReference->GridTiles.Find(Index);
	if (!IndexTileData)
	{
		UE_LOG(Log_Grid, Error, TEXT("GetValidTileNeighbors: Index (%d,%d) not found in GridTiles!"), Index.X, Index.Y);
		return Result;
	}
	
	float IndexTileHeight = IndexTileData->Transform.GetLocation().Z;
	TArray<FIntPoint> DiagonalsArray;
	TArray<FIntPoint> OrthogonalArray = GetNeighborIndexes(
		Index, true, DiagonalsArray);
		
	//Loop for orthogonal array
	for (auto const i : OrthogonalArray)
	{
		FS_TileData* LocalTileData = GridReference->GridTiles.Find(i);
		if (!LocalTileData) continue;
		
		if (!ValidTypes.Contains(LocalTileData->TileType))
		//if not valid -> remove invalid neighbor
		{
			RemoveInvalidNeighbors(i, Index, DiagonalsArray);
			continue;
		}
		
		if (LocalTileData->UnitOnTile) //if unit on tile - remove, break
		{
			RemoveInvalidNeighbors(i, Index, DiagonalsArray);
			continue;
		}
		
		//Height check if difference too high - - remove and break.
		const float HeightDifference = FMath::Abs(LocalTileData->Transform.GetLocation().Z - IndexTileHeight);

		if (HeightDifference >= GridReference->FinalTileSize.Z)
		{
			RemoveInvalidNeighbors(i, Index, DiagonalsArray);
			continue;
		}

		FS_PathfindingData NewPathData;
		NewPathData.Index = LocalTileData->Index;

		// Set cost based on tile type (matching Blueprint select node logic)
		int32 CostToEnter;
		switch (LocalTileData->TileType)
		{
		case ETileType::None:
			CostToEnter = 2;
			break;
		case ETileType::Normal:
			CostToEnter = 2;
			break;
		case ETileType::Obstacle:
			CostToEnter = 0;
			break;
		case ETileType::DifficultTerrain:
			CostToEnter = 4;
			break;
		case ETileType::GreaterDiffTerrain:
			CostToEnter = 6;
			break;
		case ETileType::FlyOnly:
			CostToEnter = 2;
			break;
		default:
			CostToEnter = 2;
			break;
		}

		NewPathData.CostToEnterTile = CostToEnter;
		NewPathData.CostFromStart = 0;
		NewPathData.MinimumCostToTarget = 0;
		NewPathData.PreviousIndex = Index;

		Result.Add(NewPathData);
	}
//Loop for diagonals
	if (DiagonalsArray.Num() == 0){return Result;}
	
	for (auto const i : DiagonalsArray)
	{
		const FS_TileData* LocalTileData = GridReference->GridTiles.Find(i);
		if (!LocalTileData) continue;
		
		if (!ValidTypes.Contains(LocalTileData->TileType) || LocalTileData->UnitOnTile)//if not valid -> remove invalid neighbor
		{
			continue;
		}

		if (FMath::Abs(LocalTileData->Transform.GetLocation().Z - IndexTileHeight) >= GridReference->FinalTileSize.Z)
		{
			continue;
		}

		FS_PathfindingData NewPathData;
		NewPathData.Index = LocalTileData->Index;

		// Set cost based on tile type (matching Blueprint select node logic)
		int32 CostToEnter;
		switch (LocalTileData->TileType)
		{
		case ETileType::None:
			CostToEnter = 3;
			break;
		case ETileType::Normal:
			CostToEnter = 3;
			break;
		case ETileType::Obstacle:
			CostToEnter = 0;
			break;
		case ETileType::DifficultTerrain:
			CostToEnter = 6;
			break;
		case ETileType::GreaterDiffTerrain:
			CostToEnter = 9;
			break;
		case ETileType::FlyOnly:
			CostToEnter = 3;
			break;
		default:
			CostToEnter = 3;
			break;
		}

		NewPathData.CostToEnterTile = CostToEnter;
		NewPathData.CostFromStart = 0;
		NewPathData.MinimumCostToTarget = 0;
		NewPathData.PreviousIndex = Index;

		Result.Add(NewPathData);
	} //for loop Orthogonal end.

	return Result;
}
