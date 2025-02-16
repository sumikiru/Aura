// Author sumikiru, made after learning Druid Mechanics' Course.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	// Broadcast Initial Spell Points
	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

/**
 * 详见方法AAuraHUD::GetSpellMenuWidgetController
 * 在第一次打开SpellMenu时，会调用该方法：USpellMenuWidgetController::BindCallbacksToDependencies()
 */
void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
		{
			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		}
	);

	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 NewSpellPoints)
		{
			// Broadcast Spell Points
			SpellPointsChangedDelegate.Broadcast(NewSpellPoints);
		}
	);
}
