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

void UMovementSplineComponent::GeneratePathFromIndices(const FVector& StartLocation, const TArray<FIntPoint>& PathIndices, float CapsuleHalfHeight)
{
	// Get the Grid owner (since this component is now part of the Grid)
	AGrid* Grid = Cast<AGrid>(GetOwner());
	if (!Grid)
	{
		return;
	}

	// Clear existing spline points
	ClearSplinePoints();

	// Add starting location as first point (already at capsule half height)
	AddSplinePoint(StartLocation, ESplineCoordinateSpace::World);

	// Convert indices to world locations and add to spline
	for (int32 i = 0; i < PathIndices.Num(); i++)
	{
		const FIntPoint& Index = PathIndices[i];
		
		// Find the tile data for this index
		if (FS_TileData* TileData = Grid->GridTiles.Find(Index))
		{
			FVector WorldLocation = TileData->Transform.GetLocation();
			// Elevate by capsule half height
			FVector ElevatedWorldLocation = WorldLocation + FVector(0, 0, CapsuleHalfHeight);
			AddSplinePoint(ElevatedWorldLocation, ESplineCoordinateSpace::World);
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