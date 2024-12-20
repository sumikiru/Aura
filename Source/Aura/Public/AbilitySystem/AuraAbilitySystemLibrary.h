// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

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
	/**
	 * 静态函数无法访问世界中的任何对象，因为静态函数所属的类本身可能在世界中并不存在，所以需要使用一个世界中已经存在的物体来追踪到HUD
	 * @param WorldContextObject 一个世界中已经存在的物体，例如WBP_Overlay
	 * @return 追踪到的OverlayWidgetController
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	/**
	 * 获取AttributeMenu的WidgetController
	 * @param WorldContextObject 一个世界中已经存在的物体，是一个UObject的常量指针
	 * @return 追踪到的AttributeMenuWidgetController
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	/**
	 * 初始化角色属性值
	 * @param WorldContextObject 一个世界中已经存在的物体，是一个UObject的常量指针
	 * @param CharacterClass 角色类型，比如ECharacterClass::Elementalist
	 * @param Level
	 * @param ASC
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

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

	/** 是否BlockHit */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);
	/** 是否造成暴击 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

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
	/** 判断攻击的角色是否为友军 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(const AActor* FirstActor, const AActor* SecondActor);
};

// TODO:使用C++实现ForLoop_WithDelay和ForEachLoop_WithDelay宏
//#define Aura_ForLoop_WithDelay(i) \
