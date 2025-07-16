// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_SpendAction.generated.h"

/**
 * Instant gameplay effect that reduces action count by a specified amount
 */
UCLASS()
class GASLEARNING_API UGE_SpendAction : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_SpendAction();
};