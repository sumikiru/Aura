// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

struct FTaggedMontage;
/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cause Damage")
	void CauseDamage(AActor* TargetActor, const FVector RadialDamageOrigin = FVector::ZeroVector);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr,
	                                                            const FVector& RadialDamageOrigin = FVector::ZeroVector) const;

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffChance = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffDamage = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffFrequency = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffDuration = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Impulse")
	float DeathImpulseMagnitude = 15000.f; // 不设置FVector DeathImpulse
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Impulse")
	float KnockbackForceMagnitude = 400.f; // 击退（未死亡时）
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Impulse")
	float KnockbackChance = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Radial Damage")
	bool bIsRadialDamage = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Radial Damage")
	float RadialDamageInnerRadius = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Radial Damage")
	float RadialDamageOuterRadius = 0.f;

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;
};
