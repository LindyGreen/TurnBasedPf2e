#pragma once

#include "CoreMinimal.h"
#include "FS_IntPointArray.generated.h"

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_IntPointArray
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="IntPoint Array"))
	TArray<FIntPoint> IntPointArray;

	FS_IntPointArray() //This is done so the int point array will be usable in blueprints. Some nesting bug.
	{
		IntPointArray = TArray<FIntPoint>();
	}
};