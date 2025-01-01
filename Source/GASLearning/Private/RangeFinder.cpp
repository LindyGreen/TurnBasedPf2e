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
	GeneratePossibleArray((10, 10), (10, 10), 10, EAE_SpellPattern_Invalid);
	// ...
}


// Called every frame
void URangeFinder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<FIntPoint> URangeFinder::GeneratePossibleArray(FIntPoint OriginPoint, FIntPoint CasterLocation, int32 Range,
                                                      EAE_SpellPattern Enum)
{
	switch (Enum)
	{
	case EAE_SpellPattern_Invalid:
		break;
	case EAE_SpellPattern_Burst:
		break;
	case EAE_SPellPattern_Line:
		return TArray<FIntPoint>(GenerateLine(CasterLocation, OriginPoint, Range));
	case EAE_SpellPattern_Cone:
		return TArray<FIntPoint>(GenerateCone(OriginPoint, CasterLocation, Range));
	case EAE_SpellPattern_Emanation:
		return TArray<FIntPoint>(GenerateEmanation(OriginPoint, Range)); //SHOULD CHANGE TO CASTER LOCATION In DEV
	}
	return TArray<FIntPoint>();
}

TArray<FIntPoint> URangeFinder::GenerateEmanation(FIntPoint OriginPoint, int32 Range)
{
	TArray<FIntPoint> ValidTiles;
	for (int32 dx = -Range; dx <= Range; dx++)
	{
		for (int32 dy = -Range; dy <= Range; dy++)
		{
			FIntPoint CurrentTile(OriginPoint.X + dx, OriginPoint.Y + dy);
			if (TotalCost(OriginPoint, CurrentTile) <= Range)
			{
				ValidTiles.Add(CurrentTile);
			}
		}
	}
	return ValidTiles;
}

TArray<FIntPoint> URangeFinder::GenerateCone(FIntPoint OriginPoint, FIntPoint CasterLocation, int32 Range)
{
	int32 ConeRange = Range-1;//to count in origin point.
	TArray<FIntPoint> ValidTiles;//init return
	FIntPoint RelativePosition = OriginPoint - CasterLocation;//init RelativeToCaster
	bool bIsOrthogonal = abs(RelativePosition.X + RelativePosition.Y) == 1;
	if (bIsOrthogonal)
	{
		//SETUP THE DIRECTION TO SHOOT AT// this will set up cone direction.
		int32 startX, endX, startY, endY;
    
		if (RelativePosition.X == 1)  // Shooting top
		{
			startX = 0;
			endX = ConeRange;
			startY = -ConeRange;
			endY = ConeRange;
		}
		else if (RelativePosition.X == -1)  // Shooting Down
		{
			startX = -ConeRange;
			endX = 0;
			startY = -ConeRange;
			endY = ConeRange;
		}
		else if (RelativePosition.Y == 1)  // Shooting Left
		{
			startX = -ConeRange;
			endX = ConeRange;
			startY = 0;
			endY = ConeRange;
		}
		else  // Shooting down
		{
			startX = -ConeRange;
			endX = ConeRange;
			startY = -ConeRange;
			endY = 0;
		}

		for (int32 dx = startX; dx <= endX; dx++)
			
		{
			for (int32 dy = startY; dy <= endY; dy++)
			{
				FIntPoint CurrentTile(OriginPoint.X + dx, OriginPoint.Y + dy);
				FIntPoint RelativeTile(CurrentTile-OriginPoint);
				if ((TotalCost(CasterLocation, CurrentTile) <= Range) && ((RelativePosition.X==0) ? abs(RelativeTile.X)<=abs(RelativeTile.Y): abs(RelativeTile.Y)<=abs(RelativeTile.X)))
				{
					ValidTiles.Add(CurrentTile);
				}
			}
		}
		

		
		UE_LOG(LogTemp, Error, TEXT("URangeFinder::GenerateCone - OriginPoint is orthogonal"));
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
				if (TotalCost(OriginPoint, CurrentTile) <= ConeRange)
				{
					ValidTiles.Add(CurrentTile);
				}
			}
		}
	}
	return ValidTiles;
}

int32 URangeFinder::TotalCost(FIntPoint OriginPoint, FIntPoint CurrentTile)//MIN Distance Calculation
{
	return FMath::Max(abs(OriginPoint.X - CurrentTile.X), abs(OriginPoint.Y - CurrentTile.Y)) +
						 FMath::Min(abs(OriginPoint.X - CurrentTile.X), abs(OriginPoint.Y - CurrentTile.Y)) / 2; 
}//Min Cost calc
TArray<FIntPoint> URangeFinder::GenerateLine(FIntPoint CasterLocation, FIntPoint OriginPoint, int32 Range)
{
	TArray<FIntPoint> ValidTiles;
	if (TotalCost(CasterLocation, OriginPoint) > Range)//return empty array of total cost>range
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

