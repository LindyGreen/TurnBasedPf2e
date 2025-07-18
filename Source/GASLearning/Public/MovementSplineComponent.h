#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "MovementSplineComponent.generated.h"

class AGrid;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GASLEARNING_API UMovementSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	UMovementSplineComponent();


	UFUNCTION(BlueprintCallable, Category = "Movement")
	void GeneratePathFromIndices(const FVector& StartLocation, const TArray<FIntPoint>& PathIndices, float CapsuleHalfHeight = 50.0f);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ClearPath();
};