// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RangeFinderLibrary.generated.h"


UENUM(BlueprintType)
enum EAE_SpellPattern : uint8
{
	EAE_SpellPattern_Invalid = 0 UMETA(DisplayName = "Debug"),
	EAE_SpellPattern_Burst=1 UMETA(DisplayName = "Burst Pattern"),
	EAE_SPellPattern_Line=2 UMETA(DisplayName = "Line Pattern"),
	EAE_SpellPattern_Cone=3 UMETA(DisplayName = "Cone Pattern"),
	EAE_SpellPattern_Emanation=4 UMETA(DisplayName = "Emanation Pattern"),
};
/** Please add a struct description */
USTRUCT(BlueprintType)
struct FS_PathfindingData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Index", MakeStructureDefaultValue="(X=-999,Y=-999)"))
	FIntPoint Index=FIntPoint(-999,-999);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="CostToEnterTile", MakeStructureDefaultValue="0"))
	int32 CostToEnterTile=0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="CostFromStart", MakeStructureDefaultValue="0"))
	int32 CostFromStart=0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="MinimumCostToTarget", MakeStructureDefaultValue="0"))
	int32 MinimumCostToTarget=0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="PreviousIndex", MakeStructureDefaultValue="(X=-999,Y=-999)"))
	FIntPoint PreviousIndex=FIntPoint(-999,-999);
};

/**
 * 
 */
UCLASS()
class GASLEARNING_API URangeFinderLibrary : public UObject
{
	GENERATED_BODY()
};
