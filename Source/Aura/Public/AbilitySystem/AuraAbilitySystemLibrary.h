// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

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
};
