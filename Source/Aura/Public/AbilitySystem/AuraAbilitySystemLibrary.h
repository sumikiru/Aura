// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAuraDamageGameplayAbility;
struct FGameplayTag;
struct FDamageEffectParams;
class UAbilityInfo;
class AAuraHUD;
struct FWidgetControllerParams;
class USpellMenuWidgetController;
struct FGameplayEffectContextHandle;
class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Widget Controller=====================================================================================
	/**
	 * 构造一个WidgetControllerParams，用于AuraHUD->GetXXXWidgetController()
	 * @param WorldContextObject 一个世界中已经存在的物体，例如WBP_SpellMenu
	 * @param OutWidgetControllerParams 用于构造的FWidgetControllerParams
	 * @param OutAuraHUD 追踪到的AuraHUD
	 * @return 是否构造成功
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWidgetControllerParams,
	                                       AAuraHUD*& OutAuraHUD);

	/**
	 * 静态函数无法访问世界中的任何对象，因为静态函数所属的类本身可能在世界中并不存在，所以需要使用一个世界中已经存在的物体来追踪到HUD
	 * @param WorldContextObject 一个世界中已经存在的物体，例如WBP_Overlay
	 * @return 追踪到的OverlayWidgetController
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	/**
	 * 获取AttributeMenu的WidgetController
	 * @param WorldContextObject 一个世界中已经存在的物体，是一个UObject的常量指针
	 * @return 追踪到的AttributeMenuWidgetController
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	/**
	 * 获取SpellMenu的WidgetController
	 * @param WorldContextObject 一个世界中已经存在的物体，是一个UObject的常量指针
	 * @return 追踪到的SpellWidgetController
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	// Ability System Class Defaults============================================================================
	/**
	 * 初始化角色属性值
	 * @param WorldContextObject 一个世界中已经存在的物体，是一个UObject的常量指针
	 * @param CharacterClass 角色类型，比如ECharacterClass::Elementalist
	 * @param Level
	 * @param ASC
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level,
	                                        UAbilitySystemComponent* ASC);

	/**
	 * 初始化角色能力
	 * @param WorldContextObject 一个世界中已经存在的物体，是一个UObject的常量指针
	 * @param ASC 能力系统组件
	 * @param CharacterClass 角色类型，比如ECharacterClass::Elementalist
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	/**
	 * 初始化角色能力
	 * @param WorldContextObject 一个世界中已经存在的物体，是一个UObject的常量指针
	 * @return 角色类信息
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	/**
	 * 获取角色技能信息
	 * @param WorldContextObject 一个世界中已经存在的物体，是一个UObject的常量指针
	 * @return 角色技能信息
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	// Effect Context Getters================================================================================
	/** 是否BlockHit */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 是否造成暴击 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 是否成功添加Debuff */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 获取Debuff伤害值 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 获取Debuff频率 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 获取Debuff持续时间 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 获取伤害类型的GameplayTag */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 获取死亡时受到的冲击方向 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 获取非死亡情况下受到的冲击方向 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 是否为范围伤害 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 获取范围伤害的内半径，在该半径内所有目标都会受到完整伤害 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 获取范围伤害的外半径，超过这个距离的目标收到最小伤害，如果最小伤害设置为0，则圈外不受伤害 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 获取范围伤害的伤害源中心位置 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);

	// Effect Context Setters================================================================================
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedHit);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalHit);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsSuccessfulDebuff);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const float InDebuffDamage);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const float InDebuffFrequency);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const float InDebuffDuration);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetKnockbackForce(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockbackForce);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsRadialDamage);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const float InRadialDamageInnerRadius);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const float InRadialDamageOuterRadius);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InRadialDamageOrigin);

	// Gameplay Mechanics=====================================================================================
	/**
	 * 获取特定半径内所有存活的角色
	 * @param WorldContextObject 一个世界中已经存在的物体，是一个UObject的常量指针
	 * @param OutOverlappingActors 用于存储特定半径内所有存活角色的数组
	 * @param ActorsToIgnore 应该忽略的对象
	 * @param Radius 特定半径
	 * @param SphereOriginLocation 中心点位置。以该点为圆心，Radius为半径的圆内寻找存活角色
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors,
	                                       const TArray<AActor*>& ActorsToIgnore, const float Radius,
	                                       const FVector& SphereOriginLocation);
	/**
	 * 获取所有目标中最近的几个目标
	 * @param MaxTargets 最大目标数量
	 * @param Actors 所有目标数组
	 * @param OutClosestTargets 记录下的最近目标数组
	 * @param Origin 寻找半径的中心点位置，以该点为原点寻找目标
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin);
	/** 判断攻击的角色是否为友军 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(const AActor* FirstActor, const AActor* SecondActor);

	/**
	 * 计算均匀分布的多段角度
	 * @param Forward 向前方向（正方向）
	 * @param Axis 基于旋转的轴，比如(0, 0, 1)
	 * @param SpreadAngleDeg 总范围的角度
	 * @param NumRotators 分成多少段
	 * @return 计算后得出的均匀分布的FRotator数组
	 * @note 常用于计算投掷物的朝向
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FRotator> EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, const float SpreadAngleDeg, const int32 NumRotators);

	/**
	 * 计算均匀分布的多段朝向
	 * @param Forward 向前方向（正方向）
	 * @param Axis 基于旋转的轴，比如(0, 0, 1)
	 * @param SpreadAngleDeg 总范围的角度
	 * @param NumVectors 分成多少段
	 * @return 计算后得出的均匀分布的FVector数组
	 * @note 常用于计算投掷物的朝向
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, const float SpreadAngleDeg, const int32 NumVectors);

	/**
	 * 应用Debuff效果
	 * @param DamageEffectParams 负面效果结构体，存储了应用Debuff时需要用到的所有数据
	 * @return 一个GameplayEffectContextHandle，用于安全传递 GameplayEffectContext，处理内存和网络传输。
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);

	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);

	/** Damage Effect Params */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static void SetRadialDamageEffectParam(UPARAM(ref) FDamageEffectParams& DamageEffectParams, const bool bIsRadial, const float InnerRadius,
	                                       const float OuterRadius, const FVector& Origin);
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static void SetKnockbackDirection(UPARAM(ref) FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude = 0.f);
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static void SetDeathImpulseDirection(UPARAM(ref) FDamageEffectParams& DamageEffectParams, FVector DeathImpulseDirection, float Magnitude = 0.f);
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static void SetTargetEffectParamsASC(UPARAM(ref) FDamageEffectParams& DamageEffectParams, UAbilitySystemComponent* InASC);
};

// TODO:使用C++实现ForLoop_WithDelay和ForEachLoop_WithDelay宏
//#define Aura_ForLoop_WithDelay(i) \

