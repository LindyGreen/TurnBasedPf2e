#include "GAS/PF2eDamageExecCalculation.h"
#include "GAS/CombatAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "LogTypes.h"

// Struct to hold attribute capture definitions
struct FPF2eDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);

	FPF2eDamageStatics()
	{
		// Capture IncomingDamage from target to output final damage
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCombatAttributeSet, IncomingDamage, Target, false);
		
		// Capture target health info for potential calculations
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCombatAttributeSet, Health, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCombatAttributeSet, MaxHealth, Target, true);
	}
};

static const FPF2eDamageStatics& DamageStatics()
{
	static FPF2eDamageStatics DStatics;
	return DStatics;
}

UPF2eDamageExecCalculation::UPF2eDamageExecCalculation()
{
	// Add the attributes we want to capture
	RelevantAttributesToCapture.Add(DamageStatics().IncomingDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().MaxHealthDef);
}

void UPF2eDamageExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	// Get source and target ASCs
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	
	if (!SourceASC || !TargetASC)
	{
		UE_LOG(Log_Combat, Warning, TEXT("PF2eDamageExecCalculation: Missing source or target ASC"));
		return;
	}
	
	
	// Get base damage from SetByCaller
	const FGameplayTag DamageDataTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
	float BaseDamage = FMath::Max<float>(Spec.GetSetByCallerMagnitude(DamageDataTag, false, -1.0f), 0.0f);
	
	if (BaseDamage <= 0.0f)
	{
		UE_LOG(Log_Combat, Warning, TEXT("PF2eDamageExecCalculation: No damage data found in SetByCaller"));
		return;
	}
	
	UE_LOG(Log_Combat, Log, TEXT("PF2eDamageExecCalculation: Processing %f base damage"), BaseDamage);
	
	// Get single damage type from the effect
	FGameplayTag DamageType = GetDamageTypeFromSpec(Spec);
	if (!DamageType.IsValid())
	{
		UE_LOG(Log_Combat, Warning, TEXT("PF2eDamageExecCalculation: No damage type found on effect"));
		DamageType = FGameplayTag::RequestGameplayTag(FName("Damage.Untyped"));
	}
	
	// Check for critical hit
	bool bIsCrit = IsCriticalHit(Spec);
	if (bIsCrit)
	{
		BaseDamage *= 2.0f;
		BaseDamage += ApplyDeadlyTrait(BaseDamage, Spec);
		UE_LOG(Log_Combat, Log, TEXT("Critical hit! Damage doubled to %f"), BaseDamage);
	}
	
	// Apply resistances/weaknesses for single damage type
	float FinalDamage = ApplyResistanceAndWeakness(BaseDamage, DamageType, ExecutionParams);
	
	UE_LOG(Log_Combat, Log, TEXT("Damage type %s: %f -> %f after resistances"), 
		*DamageType.ToString(), BaseDamage, FinalDamage);
	
	// Ensure damage is non-negative
	FinalDamage = FMath::Max(0.0f, FinalDamage);
	
	UE_LOG(Log_Combat, Log, TEXT("Final damage: %f"), FinalDamage);
	
	// Log damage taken by target
	if (TargetASC && TargetASC->GetAvatarActor())
	{
		UE_LOG(Log_Combat, Warning, TEXT("%s took %f damage"), 
			*TargetASC->GetAvatarActor()->GetName(), FinalDamage);
	}
	
	// Output the result to IncomingDamage Meta Attribute
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().IncomingDamageProperty, EGameplayModOp::Additive, FinalDamage));
}

int32 UPF2eDamageExecCalculation::GetResistanceValue(FGameplayTag DamageType, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const
{
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC)
	{
		return 0;
	}

	// Map damage types to resistance attributes
	FString DamageTypeStr = DamageType.ToString();
	
	if (DamageTypeStr == TEXT("Damage.Physical.Bludgeoning"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistanceBludgeoningAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Physical.Piercing"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistancePiercingAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Physical.Slashing"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistanceSlashingAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Energy.Fire"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistanceFireAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Energy.Cold"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistanceColdAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Energy.Electricity"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistanceElectricityAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Energy.Acid"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistanceAcidAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Energy.Sonic"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistanceSonicAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Force"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistanceForceAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Negative"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistanceNecroticAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Poison"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistancePoisonAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Mental"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetResistanceMentalAttribute()));
	}
	
	return 0; // No resistance found for this damage type
}

int32 UPF2eDamageExecCalculation::GetWeaknessValue(FGameplayTag DamageType, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const
{
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC)
	{
		return 0;
	}

	// Map damage types to weakness attributes
	FString DamageTypeStr = DamageType.ToString();
	
	if (DamageTypeStr == TEXT("Damage.Physical.Bludgeoning"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessBludgeoningAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Physical.Piercing"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessPiercingAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Physical.Slashing"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessSlashingAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Energy.Fire"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessFireAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Energy.Cold"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessColdAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Energy.Electricity"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessElectricityAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Energy.Acid"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessAcidAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Energy.Sonic"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessSonicAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Force"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessForceAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Negative"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessNecroticAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Poison"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessPoisonAttribute()));
	}
	else if (DamageTypeStr == TEXT("Damage.Mental"))
	{
		return FMath::RoundToInt(TargetASC->GetNumericAttribute(UCombatAttributeSet::GetWeaknessMentalAttribute()));
	}
	
	return 0; // No weakness found for this damage type
}

float UPF2eDamageExecCalculation::ApplyResistanceAndWeakness(float BaseDamage, FGameplayTag DamageType, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const
{
	float ModifiedDamage = BaseDamage;
	
	// Apply resistance (subtract from damage) - if resistance >= 100, it's immunity
	int32 ResistanceValue = GetResistanceValue(DamageType, ExecutionParams);
	if (ResistanceValue > 0)
	{
		ModifiedDamage = FMath::Max(0.0f, ModifiedDamage - ResistanceValue);
		{
			UE_LOG(Log_Combat, Log, TEXT("Applied %d resistance to %s damage: %f -> %f"), 
				ResistanceValue, *DamageType.ToString(), BaseDamage, ModifiedDamage);
		}
	}
	
	// Apply weakness (add to damage)
	int32 WeaknessValue = GetWeaknessValue(DamageType, ExecutionParams);
	if (WeaknessValue > 0)
	{
		ModifiedDamage += WeaknessValue;
		UE_LOG(Log_Combat, Log, TEXT("Applied %d weakness to %s damage: %f -> %f"), 
			WeaknessValue, *DamageType.ToString(), ModifiedDamage - WeaknessValue, ModifiedDamage);
	}
	
	return ModifiedDamage;
}

bool UPF2eDamageExecCalculation::IsCriticalHit(const FGameplayEffectSpec& Spec) const
{
	// Check for critical hit tag on the effect
	const FGameplayTag CritTag = FGameplayTag::RequestGameplayTag(FName("Effect.CriticalHit"));
	return Spec.GetDynamicAssetTags().HasTag(CritTag);
}

float UPF2eDamageExecCalculation::ApplyDeadlyTrait(float BaseDamage, const FGameplayEffectSpec& Spec) const
{
	// Look for specific deadly trait tags
	const FGameplayTag DeadlyD6Tag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trait.Deadly.d6"));
	const FGameplayTag DeadlyD8Tag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trait.Deadly.d8"));
	const FGameplayTag DeadlyD10Tag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trait.Deadly.d10"));
	const FGameplayTag DeadlyD12Tag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trait.Deadly.d12"));
	
	int32 DieSize = 0;
	
	// Check which deadly die size this weapon has
	if (Spec.GetDynamicAssetTags().HasTag(DeadlyD6Tag))
	{
		DieSize = 6;
	}
	else if (Spec.GetDynamicAssetTags().HasTag(DeadlyD8Tag))
	{
		DieSize = 8;
	}
	else if (Spec.GetDynamicAssetTags().HasTag(DeadlyD10Tag))
	{
		DieSize = 10;
	}
	else if (Spec.GetDynamicAssetTags().HasTag(DeadlyD12Tag))
	{
		DieSize = 12;
	}
	
	if (DieSize == 0)
	{
		return 0.0f; // No deadly trait found
	}
	
	// Roll deadly die (1 to DieSize)
	float ExtraDamage = FMath::RandRange(1, DieSize);
	UE_LOG(Log_Combat, Log, TEXT("Applied deadly d%d trait: +%f damage"), DieSize, ExtraDamage);
	return ExtraDamage;
}

FGameplayTag UPF2eDamageExecCalculation::GetDamageTypeFromSpec(const FGameplayEffectSpec& Spec) const
{
	// Look for damage type tag in the effect's dynamic asset tags (return first one found)
	const FGameplayTag DamageParentTag = FGameplayTag::RequestGameplayTag(FName("Damage"));
	
	for (const FGameplayTag& Tag : Spec.GetDynamicAssetTags())
	{
		if (Tag.MatchesTag(DamageParentTag))
		{
			return Tag; // Return first damage type found
		}
	}
	
	return FGameplayTag(); // Return invalid tag if no damage type found
}
