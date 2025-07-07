// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FS_IntPointArray.h"
#include "FL_Spells_CPP.generated.h"

class AGrid;

/**
 * 
 */
UCLASS()
class GASLEARNING_API UFL_Spells_CPP : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Spells")
	static FS_IntPointArray LineTraceSpells(const FS_IntPointArray& InputIntpointArray, AGrid* Grid, FIntPoint Origin);
};
