#include "GridMeshInstance.h"
#include "GASLearning/Public/StructsAndEnums/F_TileData.h"
#include "LogTypes.h"//Loggers are commented for future debugging purposes
#include <Components/InstancedStaticMeshComponent.h>


// Sets default values for this component's properties
UGridMeshInstance::UGridMeshInstance()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	PrimaryComponentTick.bCanEverTick = false;
	InstancedMeshComponent = CreateDefaultSubobject<
		UInstancedStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	InstancedMeshComponent->SetupAttachment(this);
	InstancedMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InstancedMeshComponent->SetNumCustomDataFloats(4); // For material usage
	// ...
}
#pragma region InstancedMesh
//This code is about initializing grid mesh, clearing all instances, removing specific, and adding them (not coloring, coloring is done in BP) 

void UGridMeshInstance::InitializeGridMeshInst(UStaticMesh* Mesh, UMaterialInterface* Material,
                                               TEnumAsByte<ECollisionEnabled::Type> Collision, bool ColorBasedOnTileType)
{
	//Initializes the Grid Mesh instance (note: this function will be called in BP, as it uses GetShapeData from FL and connects to BP_Grid)
	if (InstancedMeshComponent)
	{
		InstancedMeshComponent->SetStaticMesh(Mesh);
		InstancedMeshComponent->SetMaterial(0, Material);
		InstancedMeshComponent->SetCollisionEnabled(Collision);
		bIsColorBasedOnTileType = ColorBasedOnTileType;
	}
}

void UGridMeshInstance::ClearInstances()
{
	//Clears the Meshes and the map
	InstancedMeshComponent->ClearInstances();
	InstanceIndexes.Empty();
}

void UGridMeshInstance::RemoveInstance(FIntPoint IndexToRemove)
{
	//remove specific index
	/*UE_LOG(Log_Grid, Log,
	       TEXT(
		       "GridMeshInstance::RemoveInstance - Attempting to remove TileIndex: (%d, %d)"
	       ),
	       IndexToRemove.X, IndexToRemove.Y);*/

	if (int32* MeshInstanceIndex = InstanceIndexes.Find(IndexToRemove))
	{
		//checks if the index is contained and removes the mesh instance
		int32 RemovedIndex = *MeshInstanceIndex;
		/*UE_LOG(Log_Grid, Log,
		       TEXT(
			       "GridMeshInstance::RemoveInstance - Found MeshIndex: %d for TileIndex: (%d, %d)"
		       ),
		       RemovedIndex, IndexToRemove.X, IndexToRemove.Y);*/

		if (InstancedMeshComponent->RemoveInstance(RemovedIndex))
		{
			InstanceIndexes.Remove(IndexToRemove);

			// Update ALL other indices that were shifted down after removal
			for (auto& Pair : InstanceIndexes)
			{
				if (Pair.Value > RemovedIndex)
				{
					Pair.Value--; // Shift down by 1
				}
			}

			/*UE_LOG(Log_Grid, Log,
			       TEXT(
				       "GridMeshInstance::RemoveInstance - Removed instance, Total instances: %d"
			       ),
			       InstanceIndexes.Num());*/
		}
		else
		{
//			UE_LOG(Log_Grid, Log,TEXT("GridMeshInstance::RemoveInstance - Failed to remove MeshIndex: %d"), RemovedIndex);
		}
	}
	else
	{
//		UE_LOG(Log_Grid, Log,
//		       TEXT("GridMeshInstance::RemoveInstance - TileIndex (%d, %d) not found in InstanceIndexes"),IndexToRemove.X, IndexToRemove.Y);
	}
}

void UGridMeshInstance::AddInstance(FS_TileData TileData, int32& AddedIndex)
{
	//This is only code part of adding instance. Coloring is done in BP, Added index is used for later BP manipulation

//	UE_LOG(Log_Grid, Log, TEXT("GridMeshInstance::AddInstance - TileIndex: (%d, %d), TileType: %d"),TileData.Index.X, TileData.Index.Y, (int32)TileData.TileType);

	if (InstanceIndexes.Contains(TileData.Index))
	{
//		UE_LOG(Log_Grid, Log,
//		       TEXT("GridMeshInstance::AddInstance - Removing existing instance at (%d, %d)),TileData.Index.X, TileData.Index.Y);
		RemoveInstance(TileData.Index);
	}

	AddedIndex = InstancedMeshComponent->AddInstance(TileData.Transform);
	InstanceIndexes.Add(TileData.Index, AddedIndex);

//	UE_LOG(Log_Grid, Log,
//	       TEXT("GridMeshInstance::AddInstance - Added instance at (%d, %d) with MeshIndex: %d, Total instances: %d"),TileData.Index.X, TileData.Index.Y, AddedIndex,InstanceIndexes.Num());

	ColorTile(TileData, AddedIndex);
}

void UGridMeshInstance::UpdateTileVisual(FS_TileData DataInput)
{
//	UE_LOG(Log_Grid, Log,TEXT("GridMeshInstance::UpdateTileVisual - TileIndex:(%d, %d), TileType: %d, States: %d"),DataInput.Index.X, DataInput.Index.Y, (int32)DataInput.TileType,DataInput.TileStates.Num());


	if (int32* MeshInstanceIndex = InstanceIndexes.Find(DataInput.Index))
	{
		int32 IndexToColor = *MeshInstanceIndex;
		ColorTile(DataInput, IndexToColor);
	}
	else
	{
		// Tile to color doesn't exist, maybe need to create it first
	//	UE_LOG(Log_Grid, Log,TEXT("UpdateTileVisual: Tile (%d, %d) not found in InstanceIndexes"),DataInput.Index.X, DataInput.Index.Y);
		//	RemoveInstance(DataInput.Index);
		if (DataInput.TileType != ETileType::None && DataInput.TileType !=ETileType::Obstacle)
		{
			int32 AddedIndex;
			AddInstance(DataInput, AddedIndex);
		}
	}
}


#pragma endregion InstancedMesh
