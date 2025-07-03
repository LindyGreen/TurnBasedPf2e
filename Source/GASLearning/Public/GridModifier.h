#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StructsAndEnums/E_TileType.h"
#include "GridModifier.generated.h"

class UMaterialInstance;
UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API AGridModifier : public AActor
{
    GENERATED_BODY()

public:
    AGridModifier();

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Modifier")
    ETileType TileType = ETileType::Normal;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Modifier")
    bool bUseForTileHeight = false;
protected:


    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Modifier")
    bool bHiddenInGame = false;

    UPROPERTY(BlueprintReadWrite, Category = "Components")
    TObjectPtr<UStaticMeshComponent> StaticMesh;

public:
    UFUNCTION(BlueprintCallable, Category = "Grid Modifier")
    void SetUPConstruction(UStaticMesh *OutRowSM, UMaterialInstance *OutRowMeshMaterial, FVector Color);
};