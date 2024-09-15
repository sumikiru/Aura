// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

/**
 * 确保属性发生变化时，菜单会接收数据并变化 \n
 * 被调用位置AAuraHUD::GetAttributeMenuWidgetController
 */
void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	
	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
		[this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
		});
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	//优化前：使用DECLARE_DELEGATE_RetVal(FGameplayAttribute, FAttributeSignature);
	//Info.AttributeValue = Pair.Value.Execute().GetNumericValue(AS);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);//Pair.Value类型为AttrFuncPtr
	AttributeInfoDelegate.Broadcast(Info);
}
