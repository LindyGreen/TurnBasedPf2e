#pragma once

#include "CoreMinimal.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimInstance.h"
#include "Engine/Texture2D.h"
#include "FS_CombatantDataAssets.generated.h"

USTRUCT(BlueprintType)
struct GASLEARNING_API FS_CombatantDataAssets
{
	GENERATED_BODY()

public:
	// Visual representation assets
	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta=(DisplayName="Skeletal Mesh"))
	TSoftObjectPtr<USkeletalMesh>SkeletalMesh;

	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta=(DisplayName="Character Portrait"))
	TSoftObjectPtr<UTexture2D>CharacterPortrait;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Animation Instance Class"))
	TSubclassOf<UAnimInstance> AnimInstanceClass;

	FS_CombatantDataAssets()
	{
		SkeletalMesh = nullptr;
		AnimInstanceClass = nullptr;
		CharacterPortrait = nullptr;
	}
};