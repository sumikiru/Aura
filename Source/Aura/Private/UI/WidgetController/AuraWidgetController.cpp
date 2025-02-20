// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}

void UAuraWidgetController::UnbindingAllDelegates()
{
	//Unbind Delegate: 1.防止内存泄漏；2.避免无效调用；3.防止重复绑定
	AbilityInfoDelegate.Clear();
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	if (!GetAuraASC()->bStartupAbilitiesGiven)
	{
		return;
	}

	FForEachAbility BroadcastEachAbilityDelegate;
	BroadcastEachAbilityDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& AbilitySpec)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = AuraAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
			Info.StatusTag = AuraAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);
	GetAuraASC()->ForEachAbility(BroadcastEachAbilityDelegate);
}

AAuraPlayerController* UAuraWidgetController::GetAuraPC()
{
	/**
	 * 延迟加载并缓存一个 AAuraPlayerController 的引用（并非懒汉式单例）
	 * 1.避免重复转换（避免重复Cast）
	 * 2.确保有效性
	 * 3.逻辑解耦：将 PlayerController 的获取逻辑封装在 GetAuraPC() 中，调用方无需关心转换细节。
	 */
	if (AuraPlayerController == nullptr)
	{
		AuraPlayerController = Cast<AAuraPlayerController>(PlayerController);
	}
	return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS()
{
	if (AuraPlayerState == nullptr)
	{
		AuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
	}
	return AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS()
{
	if (AuraAttributeSet == nullptr)
	{
		AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	}
	return AuraAttributeSet;
}
