// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * 让AuraHUD的OverlayWidget指定当前的WidgetController，并执行事件SetWidgetController: \n
	 * OverlayWidget->SetWidgetController(WidgetController); \n
	 * 并且存储好WidgetController = InWidgetController以备使用
	 */
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	/**
	 * WidgetControllerSet(包括在SetWidgetController之中)是在蓝图的事件预构造之前执行的
	 * 控件蓝图中顺序为WidgetControllerSet->事件预构造->事件构造Beginplay
	 * UMG构造事件不是在创建对象（调用创建控件节点）时 触发的，而是调用“添加到视口”节点触发的
	 * 触发时的构造事件顺序：客人预构造->客人构造->自己预构造->自己构造，父母并没有进行构造事件的触发
	 * UE触发构造时的顺序：无父母 先客人 后自己
	 * C++构造函数的顺序：先父母 后客人 再自己
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
