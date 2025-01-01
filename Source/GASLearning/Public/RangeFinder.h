// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RangeFinderLibrary.h"
#include "RangeFinder.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GASLEARNING_API URangeFinder : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URangeFinder();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	//Functions and Variables I need.
	/*1. Origin Intpoint
	 *2. Target Intpoint
	 *3. Intpoint Array for a shape
	 *4. range
	 *5. Pattern Enum
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntPoint Origin;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntPoint Target;
	
	TArray<FIntPoint> PossibleArray;
	
	//Functions
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GeneratePossibleArray(FIntPoint OriginPoint, FIntPoint CasterLocation, int32 Range, EAE_SpellPattern Enum);
	UFUNCTION()
	TArray<FIntPoint> GenerateEmanation(FIntPoint OriginPoint, int32 Range);
	TArray<FIntPoint> GenerateCone(FIntPoint OriginPoint, FIntPoint CasterLocation, int32 Range);

	int32 TotalCost(FIntPoint OriginPoint, FIntPoint CurrentTile);
};
