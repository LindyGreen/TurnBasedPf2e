


#include "GridMeshInstance.h"

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


void UGridMeshInstance::InitializeGridMeshInst(UStaticMesh* Mesh, UMaterialInterface* Material, 
                                              TEnumAsByte<ECollisionEnabled::Type> Collision, 
                                              bool ColorBasedOnTileType)
{//Initializes the Grid Mesh instance
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

void UGridMeshInstance::AddInstance(FIntPoint IndexToAdd, FTransform Transform, int32& AddedIndex)
{//this is only code part of adding instance, coloring is done in BP, Added index is used for later BP manipulatiosn
	
 if (InstanceIndexes.Contains(IndexToAdd)) RemoveInstance(IndexToAdd);
InstancedMeshComponent->AddInstance(Transform);
	InstanceIndexes.Add(IndexToAdd);
	AddedIndex=InstanceIndexes.Find(IndexToAdd);
}


