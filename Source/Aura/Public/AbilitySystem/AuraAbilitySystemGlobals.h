// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * 注意：需要在DefaultGame.ini中指定新的AbilitySystemGlobalsClassName
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	/** 现在ASC->MakeEffectContext()中实现的便是这个函数。FGameplayEffectContext*变为了FAuraGameplayEffectContext*（又隐式转换为FGameplayEffectContext*） */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
