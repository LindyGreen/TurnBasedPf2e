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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Skeletal Mesh"))
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Animation Instance Class"))
	TSubclassOf<UAnimInstance> AnimInstanceClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Character Portrait"))
	TObjectPtr<UTexture2D> CharacterPortrait;

	FS_CombatantDataAssets()
	{
		SkeletalMesh = nullptr;
		AnimInstanceClass = nullptr;
		CharacterPortrait = nullptr;
	}
};