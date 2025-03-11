// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor, const FVector RadialDamageOrigin, bool bOverrideKnockbackDirection,
                                             FVector KnockbackDirectionOverride, bool bOverrideDeathImpulse, FVector DeathImpulseDirectionOverride,
                                             bool bOverridePitch, float PitchOverride)
{
	//生成配置
	const FDamageEffectParams& Params = MakeDamageEffectParamsFromClassDefaults(TargetActor,
	                                                                            RadialDamageOrigin,
	                                                                            bOverrideKnockbackDirection,
	                                                                            KnockbackDirectionOverride,
	                                                                            bOverrideDeathImpulse,
	                                                                            DeathImpulseDirectionOverride,
	                                                                            bOverridePitch,
	                                                                            PitchOverride);

	//通过配置项应用给目标ASC
	UAuraAbilitySystemLibrary::ApplyDamageEffect(Params);
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor, const FVector& RadialDamageOrigin,
                                                                                        bool bOverrideKnockbackDirection,
                                                                                        FVector KnockbackDirectionOverride,
                                                                                        bool bOverrideDeathImpulse,
                                                                                        FVector DeathImpulseDirectionOverride,
                                                                                        bool bOverridePitch, float PitchOverride) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DebuffDuration = DebuffDuration;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;
	SetDamageEffectParamsImpulseFromPitch(TargetActor, Params, bOverridePitch, PitchOverride, bOverrideKnockbackDirection, bOverrideDeathImpulse);
	// Knockback
	OverrideParamsKnockbackForce(Params, bOverrideKnockbackDirection, KnockbackDirectionOverride, bOverridePitch, PitchOverride);
	// DeathImpulse
	OverrideParamsDeathImpulse(Params, bOverrideDeathImpulse, DeathImpulseDirectionOverride, bOverridePitch, PitchOverride);
	// Radial Damage
	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageOrigin = RadialDamageOrigin;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
	}
	return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}
	return FTaggedMontage();
}

void UAuraDamageGameplayAbility::SetDamageEffectParamsImpulseFromPitch(AActor* TargetActor, FDamageEffectParams& Params, bool bOverridePitch,
                                                                       float PitchOverride, bool bOverrideKnockbackDirection,
                                                                       bool bOverrideDeathImpulse) const
{
	if (IsValid(TargetActor))
	{
		//获取到攻击对象和目标的朝向，并转换成角度
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}
		const FVector ToTarget = Rotation.Vector();
		if (!bOverrideKnockbackDirection)
		{
			//判断攻击是否触发击退
			if (FMath::RandRange(1, 100) < Params.KnockbackChance)
			{
				Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
			}
		}
		if (!bOverrideDeathImpulse)
		{
			Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		}
	}
}

void UAuraDamageGameplayAbility::OverrideParamsKnockbackForce(FDamageEffectParams& Params, bool bOverrideKnockbackDirection,
                                                              FVector KnockbackDirectionOverride, bool bOverridePitch, float PitchOverride) const
{
	if (bOverrideKnockbackDirection)
	{
		//当前攻击未触发击退。应该放在if (bOverrideKnockbackDirection)内部，否则会与
		//UAuraDamageGameplayAbility::SetDamageEffectParamsImpulseFromPitch中的判断重复
		if (FMath::RandRange(1, 100) >= Params.KnockbackChance)
		{
			Params.KnockbackForce = FVector::ZeroVector;
			return;
		}
		KnockbackDirectionOverride.Normalize();
		Params.KnockbackForce = KnockbackDirectionOverride * KnockbackForceMagnitude;
		if (bOverridePitch)
		{
			FRotator KnockbackRotation = KnockbackDirectionOverride.Rotation();
			KnockbackRotation.Pitch = PitchOverride;
			Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
		}
	}
}

void UAuraDamageGameplayAbility::OverrideParamsDeathImpulse(FDamageEffectParams& Params, bool bOverrideDeathImpulse,
                                                            FVector DeathImpulseDirectionOverride, bool bOverridePitch, float PitchOverride) const
{
	if (bOverrideDeathImpulse)
	{
		DeathImpulseDirectionOverride.Normalize();
		Params.DeathImpulse = DeathImpulseDirectionOverride * DeathImpulseMagnitude;
		if (bOverridePitch)
		{
			FRotator DeathImpulseRotation = DeathImpulseDirectionOverride.Rotation();
			DeathImpulseRotation.Pitch = PitchOverride;
			Params.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
		}
	}
}
