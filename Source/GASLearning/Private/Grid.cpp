


#include "Grid.h"

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

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

