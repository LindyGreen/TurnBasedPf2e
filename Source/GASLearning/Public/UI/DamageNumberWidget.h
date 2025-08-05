#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumberWidget.generated.h"

class UTextBlock;

UCLASS(BlueprintType, Blueprintable)
class GASLEARNING_API UDamageNumberWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDamageNumberWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	// Widget Components
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void ShowDamage(float DamageAmount, FVector WorldLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void PlayDamageAnimation();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void OnAnimationComplete();

	// Styling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styling")
	FLinearColor DamageColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styling")
	FLinearColor HealingColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float AnimationDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float WidgetLifetime = 3.0f;

private:
	UPROPERTY()
	FVector DamageWorldLocation;
};