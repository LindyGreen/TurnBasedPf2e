#pragma once

#include "CoreMinimal.h"
#include <Components/SceneComponent.h>
#include "GridMeshInstance.generated.h"
struct FS_TileData;
class UInstancedStaticMeshComponent;
class UStaticMesh;
class UMaterialInterface;

UCLASS(ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent))
class GASLEARNING_API UGridMeshInstance : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGridMeshInstance();
//VARIABLES
	UPROPERTY(BlueprintReadWrite, Category="InstancedStaticMesh")
	UInstancedStaticMeshComponent* InstancedMeshComponent;
	UPROPERTY(BlueprintReadWrite, Category="InstancedStaticMesh")
	TArray<FIntPoint> InstanceIndexes; 
	UPROPERTY(BlueprintReadWrite, Category="InstancedStaticMesh")
	bool bIsColorBasedOnTileType;
//FUNCTIONS
	UFUNCTION(BlueprintCallable, Category="InstancedStaticMesh")
	void InitializeGridMeshInst(UStaticMesh* Mesh, UMaterialInterface* Material, 
	                           TEnumAsByte<ECollisionEnabled::Type> Collision = ECollisionEnabled::NoCollision, 
	                           bool ColorBasedOnTileType = false);
	
	UFUNCTION(BlueprintCallable, Category="InstancedStaticMesh")
	void ClearInstances();
	UFUNCTION(BlueprintCallable, Category="InstancedStaticMesh")
	void RemoveInstance(FIntPoint IndexToRemove);
	UFUNCTION(BlueprintCallable, Category="InstancedStaticMesh")
	void AddInstance(FS_TileData TileData, int32& AddedIndex);
	UFUNCTION(BlueprintCallable, Category="InstancedStaticMesh")
	void UpdateTileVisual(FS_TileData DataInput);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="InstancedStaticMesh")
	void ColorTile(FS_TileData TileData, int32 AddedIndex);

};
