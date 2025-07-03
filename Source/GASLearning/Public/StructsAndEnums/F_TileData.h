/** Please add a struct description */
#pragma once

#include "CoreMinimal.h"
#include "E_TileType.h"
#include "E_TileState.h"
#include "F_TileData.generated.h"


USTRUCT(BlueprintType)
struct GASLEARNING_API FS_TileData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Index"))
	FIntPoint Index = FIntPoint(0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="TileType"))
	ETileType TileType = ETileType::Normal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Transform"))
	FTransform Transform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="TileStates"))
	TArray<ETileState> TileStates;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta=(DisplayName="UnitOnTile"))
	TObjectPtr<AActor> UnitOnTile;
};
