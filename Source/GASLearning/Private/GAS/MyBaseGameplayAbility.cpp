#include "GAS/MyBaseGameplayAbility.h"
#include "Combatant.h"
#include "AbilitySystemComponent.h"
#include "Grid.h"
#include "GAS/PF2eCombatLibrary.h"
#include "GAS/CombatAttributeSet.h"
#include "Architecture/Controller_TurnBased.h"
#include "LogTypes.h"
#include "GameplayTagContainer.h"

#include "Kismet/GameplayStatics.h"

UMyBaseGameplayAbility::UMyBaseGameplayAbility()
{
	// Set default values
	DisplayName = FText::FromString(TEXT("Base Ability"));
	Description = FText::FromString(TEXT("A basic ability"));
	ActionCost = 1;
	Range = 5;
	bRequiresTarget = true;
	bCanTargetSelf = false;
	bCanTargetAllies = false;
	bCanTargetEnemies = true;
	Category = EAbilityCategory::Attack;
	SpellLevel=0;
	// Default ability settings
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UMyBaseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	// Now we can safely initialize references because the ability is granted to an ASC with a valid avatar
	GridRef = Cast<AGrid>(UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass()));
	OwningCombatant = Cast<ACombatant>(GetAvatarActorFromActorInfo());
	PlayerControllerRef = Cast<AController_TurnBased>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	CachedASC = ActorInfo->AbilitySystemComponent.Get();
}

void UMyBaseGameplayAbility::CancelAllOtherActiveAbilities()
{
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	FGameplayAbilitySpecHandle CurrentHandle = GetCurrentAbilitySpecHandle();
	TArray<FGameplayAbilitySpecHandle> AbilitiesToCancel;
	
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.IsActive() && Spec.Handle != CurrentHandle)
		{
			AbilitiesToCancel.Add(Spec.Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& HandleToCancel : AbilitiesToCancel)
	{
		ASC->CancelAbilityHandle(HandleToCancel);
	}
}


float UMyBaseGameplayAbility::RollAbilityDamage() const
{
	// For spells, use the spell's own damage dice
	if (Category == EAbilityCategory::SpellAttack || Category == EAbilityCategory::Save)
	{
		return UPF2eCombatLibrary::RollDamage(BaseDamage, DamageDiceCount, 0.0f);
	}
	
	// For weapon attacks, start with weapon damage and apply ability modifiers
	float WeaponDie = OwningCombatant->CombatAttributes->GetDamageDie();
	float WeaponDiceCount = OwningCombatant->CombatAttributes->GetDamageDieCount();
	float WeaponBonus = OwningCombatant->CombatAttributes->GetDamageBonus();
	
	// Apply ability damage modifiers
	float TotalDiceCount = WeaponDiceCount + BonusDamageDice; // Extra dice (Power Attack)
	float TotalBonus = WeaponBonus + BonusDamageFlat;        // Flat bonus
	return UPF2eCombatLibrary::RollDamage(WeaponDie, TotalDiceCount, TotalBonus);
}

EDegreeOfSuccess UMyBaseGameplayAbility::RollAbilityAttack(AActor* Target, float RangePenalty) const
{
	// Calculate MAP penalty (need to pass bIsAgile parameter)
	float MAPPenalty = UPF2eCombatLibrary::CalculateMAPPenalty(CachedASC, bIsAgile); // TODO: Add agile weapon support
	UE_LOG(Log_Combat, Log, TEXT("MAP penalty: %f"), MAPPenalty);
	// Get target's AC
	ACombatant* TargetCombatant = Cast<ACombatant>(Target);
	int32 TargetAC = static_cast<int32>(TargetCombatant->GetAbilitySystemComponent()->GetNumericAttribute(UCombatAttributeSet::GetACAttribute()));
	//Add ternary operator to use spell attacks instead of physical ones
	int32 AttackBonus = OwningCombatant->CombatAttributes->GetAttackBonus();
	int32 MaxDieRoll = OwningCombatant->CombatAttributes->GetMaxDieRoll();
	
	// Combine range penalty and MAP penalty
	int32 TotalPenalty = static_cast<int32>(RangePenalty + MAPPenalty);
	
	// Roll the attack
	EDegreeOfSuccess Result;
	if (TotalPenalty > 0)
	{
		Result = UPF2eCombatLibrary::RollAttackWithPenalty(AttackBonus, static_cast<int32>(TargetAC), TotalPenalty, MaxDieRoll);
	}
	else
	{
		Result = UPF2eCombatLibrary::RollAttack(AttackBonus, static_cast<int32>(TargetAC), MaxDieRoll);
	}
	
	// Apply MAP tags after rolling attack
	ApplyMAPTags();
	
	
	return Result;
}

EDegreeOfSuccess UMyBaseGameplayAbility::RollSavingThrow(int32 TargetSaveBonus) const
{
	// Use the centralized PF2e combat library
	return UPF2eCombatLibrary::RollSave(TargetSaveBonus, SaveDC);
}

TArray<FIntPoint> UMyBaseGameplayAbility::GetAbilityArea(FIntPoint TargetLocation) const
{
	// Simple single target for now - this will be expanded when RangeFinder is available
	TArray<FIntPoint> AffectedSquares;
	
	switch (AreaType)
	{
		case ESpellArea::SingleTarget:
			AffectedSquares.Add(TargetLocation);
			break;
			
		case ESpellArea::Burst:
			// TODO: Use RangeFinder's GenerateBurst when available
			// For now, simple implementation
			AffectedSquares.Add(TargetLocation);
			break;
			
		case ESpellArea::SelfOrEmanation:
			// TODO: Use RangeFinder's GenerateEmanation when available
			// For now, simple implementation
			AffectedSquares.Add(TargetLocation);
			break;
			
		case ESpellArea::Line:
			// TODO: Use RangeFinder's GenerateLine when available
			AffectedSquares.Add(TargetLocation);
			break;
			
		case ESpellArea::Cone:
			// TODO: Use RangeFinder's GenerateCone when available
			AffectedSquares.Add(TargetLocation);
			break;
	}
	
	return AffectedSquares;
}

int32 UMyBaseGameplayAbility::CalculateDistanceToTarget(ACombatant* Target) const
{
	if (!Target || !OwningCombatant)
		return -1;
	
	FIntPoint OwnerPos = OwningCombatant->LocationIndex;
	FIntPoint TargetPos = Target->LocationIndex;
	
	//TODO replace with TOTAL COST function from Rangefinder
	return FMath::Max(FMath::Abs(OwnerPos.X - TargetPos.X), FMath::Abs(OwnerPos.Y - TargetPos.Y));
}




bool UMyBaseGameplayAbility::IsTargetInRange(ACombatant* Target, int32& OutRangePenalty) const
{
	OutRangePenalty = 0;
	
	if (!Target)
		return false;
	
	int32 Distance = CalculateDistanceToTarget(Target);
	if (Distance < 0)
		return false;
	
	// For melee attacks (Category == Attack, Range <= 1)
	if (Category == EAbilityCategory::Attack && Range <= 1)
	{
		return Distance <= Range;
	}
	
	// For ranged attacks and spells
	int32 EffectiveMaxRange = (MaxRange > 0) ? MaxRange : Range;
	
	// Check if within maximum range
	if (Distance > EffectiveMaxRange)
		return false;
	
	// Apply range penalty for ranged attacks (not spells)
	if (Category == EAbilityCategory::Attack && LongRangeThreshold > 0)
	{
		if (Distance > LongRangeThreshold * 2)
		{
			OutRangePenalty = 4; // -4 penalty for third weapon increment
		}
		else if (Distance > LongRangeThreshold)
		{
			OutRangePenalty = 2; // -2 penalty for second increment
		}
	}
	
	return true;
}

bool UMyBaseGameplayAbility::HasLineOfSight(ACombatant* Target) const
{
	// TODO: Implement line of sight checking with grid system
	// For now, assume we always have line of sight
	return true;
}

// Look-at system implementation
void UMyBaseGameplayAbility::EnableCombatantLookAt()
{
	if (!OwningCombatant || !PlayerControllerRef)
		return;
		
	// Enable look-at on combatant
	OwningCombatant->EnableLookAt();
	
	// Bind to controller's tile hover delegate
	PlayerControllerRef->OnHoveredTileUpdated.AddDynamic(this, &UMyBaseGameplayAbility::OnTileHoverChanged);
}

void UMyBaseGameplayAbility::DisableCombatantLookAt()
{
	if (!OwningCombatant || !PlayerControllerRef)
		return;
		
	// Disable look-at on combatant
	OwningCombatant->DisableLookAt();
	
	// Unbind from controller's tile hover delegate
	PlayerControllerRef->OnHoveredTileUpdated.RemoveDynamic(this, &UMyBaseGameplayAbility::OnTileHoverChanged);
}

void UMyBaseGameplayAbility::OnTileHoverChanged(FIntPoint NewTileIndex)
{
	if (!OwningCombatant || !GridRef)
		return;
		
	// Convert tile index to world location
	const FS_TileData* FoundTile = GridRef->GridTiles.Find(NewTileIndex);
	if (!FoundTile)
		return;
		
	FVector TileWorldLocation = FoundTile->Transform.GetLocation();
	
	// Tell combatant to look at this location
	OwningCombatant->SetLookAtLocation(TileWorldLocation);
}

void UMyBaseGameplayAbility::ApplyMAPTags() const
{
	if (!CachedASC)
		return;
	
	FGameplayTagContainer OwnedTags;
	CachedASC->GetOwnedGameplayTags(OwnedTags);
	
	// Add MAP tags based on current state
	if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.1"))))
	{
		// Already have MAP.1, upgrade to MAP.2
		CachedASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.1")));
		CachedASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.2")));
	}
	else if (!OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.2"))))
	{
		// First attack, add MAP.1
		CachedASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.1")));
	}
	// If already have MAP.2, don't add more tags
}
