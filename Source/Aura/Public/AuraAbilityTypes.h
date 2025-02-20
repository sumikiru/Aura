#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

/**
 * 技能负面效果参数
 */
USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams()
	{
	}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;
	UPROPERTY()
	float BaseDamage = 0.f;
	UPROPERTY()
	float AbilityLevel = 1.f;
	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();
	UPROPERTY()
	float DebuffChance = 0.f;
	UPROPERTY()
	float DebuffDamage = 0.f;
	UPROPERTY()
	float DebuffDuration = 0.f;
	UPROPERTY()
	float DebuffFrequency = 0.f;
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }

	void SetIsCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }

	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		// 注意：不应该返回FGameplayEffectContext::StaticStruct()，而是应该返回FAuraGameplayEffectContext::StaticStruct()
		return StaticStruct();
	}

	/**
	 * Creates a copy of this context, used to duplicate for later modifications \n
	 * 注意：这里不应该返回*FGameplayEffectContext，应该返回*FAuraGameplayEffectContext
	 */
	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	/** Custom serialization, subclasses must override this! */
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

protected:
	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;
};

/** struct + template的组合可以让我们使用同一个结构体名称（注意：只是名称相同，但是本质上已经不同了），实现不同的结构体功能，可以将其理解为设计模式中的工程模式。*/
template <>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true, // 开启以后才能调用NetSerialize
		WithCopy = true // 开启以后才能调用Duplicate
	};
};
