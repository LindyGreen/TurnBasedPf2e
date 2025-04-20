// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeFinder.h"


// Sets default values for this component's properties
URangeFinder::URangeFinder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URangeFinder::BeginPlay()
{
	Super::BeginPlay();
	// ...
}

// Called every frame
void URangeFinder::TickComponent(float DeltaTime, ELevelTick TickType,
                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<FIntPoint> URangeFinder::GeneratePossibleArray(
	FIntPoint OriginPoint, FIntPoint CasterLocation, uint8 Area,
	EAE_SpellPattern Enum, bool bIgnoreOrigin)
{
	switch (Enum)
	{
	case EAE_SpellPattern_Invalid:
		UE_LOG(LogTemp, Display,
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
		UE_LOG(LogTemp, Error,
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
                                          TArray<FIntPoint>& Neighborstoremove, FIntPoint Index)
{
	UE_LOG(LogTemp, Error, TEXT("%d" "%d"),CurrentArrayElement.X, CurrentArrayElement.Y );
	FIntPoint TempElement = CurrentArrayElement-Index;
	if (TempElement.X == -1)
	{
				
		Neighborstoremove={FIntPoint(Index.X=-1, Index.Y-1), FIntPoint(-1, +1)};
		return;
	}
	if (TempElement.X == 1)
	{
		
		Neighborstoremove={FIntPoint(Index.X+1, Index.Y+1), FIntPoint(Index.X+1, Index.Y-1)};
		return;
	}
	if (TempElement.Y == -1)
	{
		
		Neighborstoremove={FIntPoint(Index.X+1, Index.Y-1), FIntPoint(Index.X-1, Index.Y-1)};
		return;
	}
	if (TempElement.Y == 1)
	{
		Neighborstoremove={FIntPoint(Index.X-1, Index.Y+1), FIntPoint(Index.X+1, Index.Y+1)};
		return;
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
