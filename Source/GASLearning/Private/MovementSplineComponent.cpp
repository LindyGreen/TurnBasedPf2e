#include "MovementSplineComponent.h"
#include "Grid.h"
#include "StructsAndEnums/F_TileData.h"

UMovementSplineComponent::UMovementSplineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	// Enable debug visualization
	bDrawDebug = true;
	SetUnselectedSplineSegmentColor(FLinearColor::Green);
	SetSelectedSplineSegmentColor(FLinearColor::Yellow);
	ScaleVisualizationWidth = 5.0f;
}

void UMovementSplineComponent::GeneratePathFromIndices(const TArray<FIntPoint>& PathIndices)
{
	if (!GridRef)
	{
		return;
	}

	// Clear existing spline points
	ClearSplinePoints();

	// Add combatant's current location as starting point
	FVector StartLocation = GetOwner()->GetActorLocation();
	AddSplinePoint(StartLocation, ESplineCoordinateSpace::World);

	// Convert indices to world locations and add to spline
	for (int32 i = 0; i < PathIndices.Num(); i++)
	{
		const FIntPoint& Index = PathIndices[i];
		
		// Find the tile data for this index
		if (FS_TileData* TileData = GridRef->GridTiles.Find(Index))
		{
			FVector WorldLocation = TileData->Transform.GetLocation();
			AddSplinePoint(WorldLocation, ESplineCoordinateSpace::World);
		}
	}

	// Update the spline
	UpdateSpline();
}

void UMovementSplineComponent::ClearPath()
{
	ClearSplinePoints();
	UpdateSpline();
}