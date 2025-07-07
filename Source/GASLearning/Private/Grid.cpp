


#include "Grid.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GASLearning/Public/GridModifier.h"


// Sets default values
AGrid::AGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create and set up components
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;
	
	MeshHandle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshHandle"));
	MeshHandle->SetupAttachment(RootComponent);

}

bool AGrid::IsIndexValid(FIntPoint Index) const
{
	return GridTiles.Contains(Index);
}

FVector AGrid::GetTileLocationFromIndex(FIntPoint GridIndex) const
{
	return GridStartingPosition + FVector(GridIndex.X * FinalTileSize.X, GridIndex.Y * FinalTileSize.Y, 0.0f);
}

TArray<FIntPoint> AGrid::GetAllTilesWithState(ETileState State)
{//looks through all tiles for ones with a state
	if (const FS_IntPointArray* StateArray = TileStatesToIndexes.Find(State))
	{
		return StateArray->IntPointArray;
	}
	return TArray<FIntPoint>();
}

FVector AGrid::CalculateSquaresSpawnLocation(FIntPoint GridIndex) const
{//Calculates where to put each grid index
	return GridStartingPosition + FVector(GridIndex.X * FinalTileSize.X, GridIndex.Y * FinalTileSize.Y, 0.01f);
}

void AGrid::CalculateStartingPosition()
{ //Snaps the starting position into the correct place on a grid synced with the materials 
	// Grid snap the current GridStartingPosition to align with tile boundaries
	FVector CenterVar;
	CenterVar.X = FMath::GridSnap(GridStartingPosition.X, FinalTileSize.X);
	CenterVar.Y = FMath::GridSnap(GridStartingPosition.Y, FinalTileSize.Y);
	CenterVar.Z = GridStartingPosition.Z; // Keep original Z
	
	
	// Move the MeshHandle to the calculated position
	if (MeshHandle)
	{
		MeshHandle->SetWorldLocation(CenterVar);
	}
	// Update GridStartingPosition with snapped values
	GridStartingPosition = CenterVar;
}

FVector AGrid::TraceForGround(FVector PotentialLocation, ETileType& OutTileType)
{
	FVector TraceStart = PotentialLocation + FVector(0, 0, 1000);
	FVector TraceEnd = PotentialLocation + FVector(0, 0, -1000);
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = false;
	
	// Hit result to store the outcome of the trace
	TArray<FHitResult> OutHit;
	bool ReturnValue = GetWorld()->LineTraceMultiByChannel(OutHit, TraceStart, TraceEnd, ECC_GameTraceChannel1, TraceParams);
	
	if (!ReturnValue)
	{
		OutTileType = ETileType::None; 
		return PotentialLocation; // Return original location
	}
	
	// Setting up defaults - like BP does
	ETileType LocalTileType = ETileType::Normal;
	bIsHeightFound = false; // Reset to false like BP does at start
	
	// Local Z variable for height calculation - start with BP's default
	double LocalZ = 2.0; // Default LocalZ value from BP
	
	// Looping through OutHit
	for (const FHitResult& HitResult : OutHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (AGridModifier* GridModifier = Cast<AGridModifier>(HitActor))
		{
			// Get tile type from GridModifier
			LocalTileType = GridModifier->TileType;
			
			// Check if this modifier is used for tile height
			if (GridModifier->bUseForTileHeight)
			{
				bIsHeightFound = true;
				
				// Get hit location and snap Z coordinate using TileSize.Z and GridOffsetFromGround
				FVector HitLocation = HitResult.Location;
				double SnappedZ = UKismetMathLibrary::GridSnap_Float(HitLocation.Z, TileSize.Z);
				LocalZ = SnappedZ + GridOffsetFromGround;
			}
		}
		else
		{
			// Not a GridModifier, check if height was found
			if (!bIsHeightFound)
			{
				// Use the ground hit location
				FVector HitLocation = HitResult.Location;
				double SnappedZ = UKismetMathLibrary::GridSnap_Float(HitLocation.Z, TileSize.Z);
				LocalZ = SnappedZ + GridOffsetFromGround;
			}
		}
	}
	
	// Set the output tile type
	OutTileType = LocalTileType;
	return FVector(PotentialLocation.X, PotentialLocation.Y, LocalZ);
}

FVector AGrid::GetCursorLocationOnGrid()
{
	// Get player controller Default check if player contriller is there
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
	{
		return FVector(7014.0, 0.0, 0.0); // Default fallback location
	}

	// First attempt: Try GroundAndGridMod channel (TraceTypeQuery1)
	FHitResult HitResult;
	bool bHit = PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, HitResult);
	
	if (bHit)
	{
		FVector Result = HitResult.Location;
		return Result;
	}

	// Second attempt: Try Grid channel (TraceTypeQuery2) 
	bHit = PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2), false, HitResult);
	
	if (bHit)
	{
		FVector Result = HitResult.Location;
		return Result;
	}

	// Third attempt: Mathematical plane intersection fallback
	FVector WorldLocation, WorldDirection;
	bool bValidProjection = PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	
	if (bValidProjection)
	{
		// Create a horizontal plane at Z=0 with normal pointing up
		FPlane GroundPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(FVector(0, 0, 0), FVector(0, 0, 1));
		
		// Create a long ray by multiplying direction
		FVector LineEnd = WorldLocation + (WorldDirection * 99999999.0f);
		
		// Calculate line-plane intersection
		float T;
		FVector Intersection;
		bool bIntersects = UKismetMathLibrary::LinePlaneIntersection(WorldLocation, LineEnd, GroundPlane, T, Intersection);
		
		if (bIntersects)
		{
			return Intersection;
		}
	}

	// Final fallback
	FVector FallbackResult = FVector(7014.0, 0.0, 0.0);
	return FallbackResult;
	
}

FIntPoint AGrid::GetIndexFromWorldLocation(FVector InputLocation)
{
	// Calculate LocationOnGrid
	FVector LocationOnGrid = InputLocation - GridStartingPosition;
	
	// Direct conversion: divide by tile size and round to get grid index
	FIntPoint Result = FIntPoint(
		FMath::RoundToInt(LocationOnGrid.X / FinalTileSize.X),
		FMath::RoundToInt(LocationOnGrid.Y / FinalTileSize.Y)
	);
	
	return Result;
}

FIntPoint AGrid::GetTileIndexUnderCursor()
{
	return 	GetIndexFromWorldLocation(GetCursorLocationOnGrid());
}

FS_IntPointArray AGrid::RemoveObstacleTiles(const FS_IntPointArray& InputArray)
{
	FS_IntPointArray TempArray; // Clear temp array at start
	
	// Loop through input array
	for (const FIntPoint& TileIndex : InputArray.IntPointArray)
	{
		// Find tile in GridTiles map
		if (const FS_TileData* TileData = GridTiles.Find(TileIndex))
		{
			// If tile type is NOT Obstacle or None, add to temp array
			if (TileData->TileType != ETileType::Obstacle && TileData->TileType != ETileType::None)
			{
				TempArray.IntPointArray.Add(TileIndex);
			}
		}
	}
	
	return TempArray;
}



