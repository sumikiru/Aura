// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraArcaneShards.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraArcaneShards : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cause Damage")
	void CauseDamageToAllTargets(const FVector& RadialDamageOrigin = FVector::ZeroVector);
};
