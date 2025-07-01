#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Actor.h>
#include "Grid.generated.h"

UCLASS()
class GASLEARNING_API AGrid : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid")
	FVector GridStartingPosition;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid")
	FVector FinalTileSize;
	
};
