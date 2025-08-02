#include "GAS/MyBaseGameplayAbility.h"
#include "Combatant.h"
#include "AbilitySystemComponent.h"
#include "Grid.h"
#include "GAS/PF2eCombatLibrary.h"
#include "GAS/CombatAttributeSet.h"
#include "Architecture/Controller_TurnBased.h"
#include "LogTypes.h"
#include "GameplayTagContainer.h"
#include "RangeFinder.h"

#include "GAS/SpellsAttributeSet.h"

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
	
	// Get RangeFinder from Grid (Blueprint adds this component)
	if (GridRef)
	{
		RangeFinderRef = GridRef->FindComponentByClass<URangeFinder>();
	}
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
	int32 AttackBonus;
	if (GetAbilityCategory()==EAbilityCategory::SpellAttack)
	{
	AttackBonus = OwningCombatant->SpellsAttributes->GetSpellAttackBonus();
	}
	else
	{
	AttackBonus = OwningCombatant->CombatAttributes->GetAttackBonus();
	}
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
	
	// Add MAP tags based on current state - this sets up penalty for NEXT attack
	if (!OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.1"))) && 
		!OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.2"))))
	{
		// No MAP tags present, this is first attack - add MAP.1 for second attack
		CachedASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.1")));
	}
	else if (OwnedTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.1"))))
	{
		// MAP.1 present, this is second attack - upgrade to MAP.2 for third attack
		CachedASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.1")));
		CachedASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Conditions.MAP.2")));
	}
	// If MAP.2 is present, this is third+ attack - leave MAP.2 (max penalty reached)
}

void UMyBaseGameplayAbility::ApplyDamageToTarget(ACombatant* Target, float DamageAmount, bool bIsCriticalHit)
{
	if (!Target || !DamageGameplayEffectClass)
	{
		UE_LOG(Log_Combat, Warning, TEXT("ApplyDamageToTarget: Missing target or damage GameplayEffect class"));
		return;
	}

	UAbilitySystemComponent* TargetASC = Target->GetAbilitySystemComponent();
	if (!TargetASC)
	{
		UE_LOG(Log_Combat, Warning, TEXT("ApplyDamageToTarget: Target has no AbilitySystemComponent"));
		return;
	}

	// Create the damage effect spec
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffectClass);
	if (!DamageSpecHandle.IsValid())
	{
		UE_LOG(Log_Combat, Error, TEXT("ApplyDamageToTarget: Failed to create GameplayEffect spec"));
		return;
	}

	FGameplayEffectSpec* DamageSpec = DamageSpecHandle.Data.Get();
	
	// Set damage amount via SetByCaller
	const FGameplayTag DamageDataTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
	DamageSpec->SetSetByCallerMagnitude(DamageDataTag, DamageAmount);
	
	// Add damage type tag
	if (DamageType.IsValid())
	{
		DamageSpec->AddDynamicAssetTag(DamageType);
	}
	else
	{
		// Default to untyped if no damage type set
		DamageSpec->AddDynamicAssetTag(FGameplayTag::RequestGameplayTag(FName("Damage.Untyped")));
	}
	
	
	// Add critical hit tag if this is a crit
	if (bIsCriticalHit)
	{
		const FGameplayTag CritTag = FGameplayTag::RequestGameplayTag(FName("Effect.CriticalHit"));
		DamageSpec->AddDynamicAssetTag(CritTag);
	}
	
	// Add deadly trait if weapon has it
	if (bHasDeadlyTrait)
	{
		FGameplayTag DeadlyTag;
		switch (DeadlyDieSize)
		{
			case 6:
				DeadlyTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trait.Deadly.d6"));
				break;
			case 8:
				DeadlyTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trait.Deadly.d8"));
				break;
			case 10:
				DeadlyTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trait.Deadly.d10"));
				break;
			case 12:
				DeadlyTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trait.Deadly.d12"));
				break;
			default:
				DeadlyTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trait.Deadly.d6")); // Default to d6
				break;
		}
		
		if (DeadlyTag.IsValid())
		{
			DamageSpec->AddDynamicAssetTag(DeadlyTag);
		}
	}
	
	// Apply the damage effect to the target
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpec, TargetASC);
	
	UE_LOG(Log_Combat, Log, TEXT("Applied %f %s damage to %s (Crit: %s)"), 
		DamageAmount, 
		DamageType.IsValid() ? *DamageType.ToString() : TEXT("Untyped"),
		*Target->GetName(), 
		bIsCriticalHit ? TEXT("Yes") : TEXT("No"));
}

void UMyBaseGameplayAbility::StartAimingStage()
{
	// Clear previous "IsInRange" states from all tiles
	GridRef->ClearStateFromTiles(ETileState::IsInRange);
	GridRef->ClearStateFromTiles(ETileState::IsInRange2);
	
	// Get valid tiles for this ability using emanation pattern
	FIntPoint CasterLocation = OwningCombatant->LocationIndex;
	TArray<FIntPoint> ValidTiles = RangeFinderRef->GetEffectAreaOrRange(
		CasterLocation,           // OriginPoint
		CasterLocation,           // CasterLocation
		Range,                    // Range
		EAE_SpellPattern_Emanation, // Pattern - always emanation
		!bRequiresTarget,         // IgnoreLOS
		!bCanTargetSelf           // IgnoreOrigin
	);
	
	// Mark all valid tiles as "IsInRange"
	for (const FIntPoint& TileIndex : ValidTiles)
	{
		GridRef->AddStateToTile(TileIndex, ETileState::IsInRange);
	}
	
	// Check if second range increment should be implemented (only for non-spell attacks)
	if (Category == EAbilityCategory::Attack && Range > 2)
	{
		// Get tiles for second range increment (Range*2)
		TArray<FIntPoint> ValidRange2Tiles = RangeFinderRef->GetEffectAreaOrRange(
			CasterLocation,           // OriginPoint
			CasterLocation,           // CasterLocation
			Range * 2,                // Range*2 for second increment
			EAE_SpellPattern_Emanation, // Pattern - always emanation
			!bRequiresTarget,         // IgnoreLOS
			!bCanTargetSelf           // IgnoreOrigin
		);
		
		// Remove tiles that are already in first range (closer tiles)
		ValidRange2Tiles.RemoveAll([&ValidTiles](const FIntPoint& Tile) {
			return ValidTiles.Contains(Tile);
		});
		
		// Mark farther tiles as "IsInRange2"
		for (const FIntPoint& TileIndex : ValidRange2Tiles)
		{
			GridRef->AddStateToTile(TileIndex, ETileState::IsInRange2);
		}
	}
	
	// Set targeting flag
	bIsTargeting = true;
	
	UE_LOG(LogTemp, Log, TEXT("StartAimingStage: Highlighted %d close tiles for ability %s"), 
		ValidTiles.Num(), *GetAbilityDisplayName().ToString());
	
	EnableCombatantLookAt();
}

void UMyBaseGameplayAbility::GenerateAffectedTilesForAIEAndSingleSpells(FIntPoint OriginPoint)
{
	// Determine spell pattern based on AreaType
	EAE_SpellPattern SpellPattern;
	switch (AreaType)
	{
		case ESpellArea::SingleTarget:
		case ESpellArea::MultipleTargets:
			SpellPattern = EAE_SpellPattern_Invalid; // Debug pattern for single/multi target
			break;
		case ESpellArea::Line:
			SpellPattern = EAE_SPellPattern_Line;
			break;
		case ESpellArea::Cone:
			SpellPattern = EAE_SpellPattern_Cone;
			break;
		case ESpellArea::Burst:
			SpellPattern = EAE_SpellPattern_Burst;
			break;
		case ESpellArea::SelfOrEmanation:
			SpellPattern = EAE_SpellPattern_Emanation;
			break;
		default:
			SpellPattern = EAE_SpellPattern_Invalid;
			break;
	}
	
	// Get caster location
	FIntPoint CasterLocation = OwningCombatant->LocationIndex;
	
	// Call RangeFinder to get affected tiles and store in member variable
	TargetedTiles = RangeFinderRef->GetEffectAreaOrRange(
		OriginPoint,     // Origin
		CasterLocation,  // Caster location
		AreaSize,        // Use AreaSize as Range
		SpellPattern,    // Pattern based on AreaType
		false,           // IgnoreLOS = false
		false            // IgnoreOrigin = false
	);
}
