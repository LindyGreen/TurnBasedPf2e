


#include "GridMeshInstance.h"
#include "GASLearning/Public/StructsAndEnums/F_TileData.h"
#include <Components/InstancedStaticMeshComponent.h>


// Sets default values for this component's properties
UGridMeshInstance::UGridMeshInstance()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	InstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	InstancedMeshComponent->SetupAttachment(this);
	InstancedMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// ...
}
#pragma region InstancedMesh
//This code is about initializing grid mesh, clearing all instances, removing specific, and adding them (not coloring, coloring is done in BP) 

void UGridMeshInstance::InitializeGridMeshInst(UStaticMesh* Mesh, UMaterialInterface* Material, 
                                              TEnumAsByte<ECollisionEnabled::Type> Collision, 
                                              bool ColorBasedOnTileType)
{//Initializes the Grid Mesh instance (note: this function will be called in BP, as it uses GetShapeData from FL and connects to BP_Grid)
	if (InstancedMeshComponent)
	{
		InstancedMeshComponent->SetStaticMesh(Mesh);
		InstancedMeshComponent->SetMaterial(0, Material);
		InstancedMeshComponent->SetCollisionEnabled(Collision);
		bIsColorBasedOnTileType = ColorBasedOnTileType;
	}
}

void UGridMeshInstance::ClearInstances() 
{ //Clears the Meshes and the array
	InstancedMeshComponent->ClearInstances();
	InstanceIndexes.Empty();
}

void UGridMeshInstance::RemoveInstance(FIntPoint IndexToRemove)
{//remove specific index
	if (InstanceIndexes.Contains(IndexToRemove))
	{//checks if the index is contained and removes the index
		InstancedMeshComponent->RemoveInstance(InstanceIndexes.Find(IndexToRemove));
		InstanceIndexes.Remove(IndexToRemove);
	}
}

void UGridMeshInstance::AddInstance(FS_TileData TileData, int32& AddedIndex)
{//This is only code part of adding instance. Coloring is done in BP, Added index is used for later BP manipulation
	
 if (InstanceIndexes.Contains(TileData.Index)) RemoveInstance(TileData.Index);
InstancedMeshComponent->AddInstance(TileData.Transform, AddedIndex);
	InstanceIndexes.Add(TileData.Index);
	AddedIndex=InstanceIndexes.Find(TileData.Index);
		ColorTile(TileData, AddedIndex);
}

void UGridMeshInstance::UpdateTileVisual(FS_TileData DataInput)
{
	RemoveInstance(DataInput.Index);
	if (DataInput.TileType!=ETileType::None && DataInput.TileType!=ETileType::Obstacle)
	{
		int32 AddedIndex;
		AddInstance(DataInput, AddedIndex);
	}
}


#pragma endregion InstancedMesh



