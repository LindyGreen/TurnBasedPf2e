#include "UI/DamageNumberWidget.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "TimerManager.h"

UDamageNumberWidget::UDamageNumberWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDamageNumberWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDamageNumberWidget::ShowDamage(float DamageAmount, FVector WorldLocation)
{
	if (!DamageText)
	{
		return;
	}

	DamageWorldLocation = WorldLocation;
	
	// Format damage text
	FText DamageDisplayText;
	if (DamageAmount < 0)
	{
		// Positive values are damage
		DamageDisplayText = FText::FromString(FString::Printf(TEXT("-%d"), FMath::RoundToInt(DamageAmount)));
		DamageText->SetColorAndOpacity(DamageColor);
	}
	else if (DamageAmount > 0)
	{
		// Negative values are healing
		DamageDisplayText = FText::FromString(FString::Printf(TEXT("+%d"), FMath::RoundToInt(FMath::Abs(DamageAmount))));
		DamageText->SetColorAndOpacity(HealingColor);
	}
	else
	{
		// Zero damage
		DamageDisplayText = FText::FromString(TEXT("0"));
		DamageText->SetColorAndOpacity(DamageColor);
	}

	DamageText->SetText(DamageDisplayText);

	// Play animation (implemented in Blueprint)
	PlayDamageAnimation();

	// Auto-remove after widget lifetime (safety cleanup)
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UDamageNumberWidget::OnAnimationComplete, WidgetLifetime, false);
}

void UDamageNumberWidget::OnAnimationComplete()
{
	// Remove widget from parent and mark for garbage collection
	RemoveFromParent();
}