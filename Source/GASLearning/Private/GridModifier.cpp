#include "GridModifier.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstance.h"


AGridModifier::AGridModifier()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create and set static mesh component as root
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    RootComponent = StaticMesh;
    
}

void AGridModifier::SetUPConstruction(UStaticMesh* OutRowSM,
    UMaterialInstance* OutRowMeshMaterial, FVector Color)
{
    StaticMesh->SetStaticMesh(OutRowSM);
    StaticMesh->SetMaterial(0, OutRowMeshMaterial);
    StaticMesh->SetVectorParameterValueOnMaterials("Color", Color);
    StaticMesh->SetScalarParameterValueOnMaterials("IsFilled", 0.6);
    StaticMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);//COLLISIONCHANNEL 1 is the Ground and Grid
    SetActorHiddenInGame(bHiddenInGame);
}

