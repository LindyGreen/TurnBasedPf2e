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
/**
 * 
 */
UCLASS()
class GASLEARNING_API URangeFinderLibrary : public UObject
{
	GENERATED_BODY()
};
