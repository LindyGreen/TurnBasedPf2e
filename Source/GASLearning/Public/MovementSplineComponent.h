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

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	TObjectPtr<AGrid> GridRef = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void GeneratePathFromIndices(const TArray<FIntPoint>& PathIndices);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ClearPath();
};