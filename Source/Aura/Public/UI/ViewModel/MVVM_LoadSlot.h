// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	// 切换存档槽位显示的UI：Vacant/EnterName/Taken
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndexDelegate;
	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButtonDelegate;

	// 切换显示的存档槽位状态(根据SlotStatus)：默认显示Vacant状态下的UI
	void InitializeSlot();

	UPROPERTY()
	int32 SlotIndex; // 存档槽位索引，如LoadSlot_0设置SlotIndex为0（在蓝图的InitializeSlot中执行）

	// 强类型枚举类不需要TEnumAsByte<TEnum>这种包裹形式
	ESaveSlotStatus SlotStatus; // 当前存档插槽状态，如Vacant/EnterName/Taken

	/**
	 * Field Notifies: 适用于需要进行视图绑定的元素，关系到View
	 * 要广播对绑定到你的参数的控件的更改，你需要将变量或函数标记为 FieldNotify 。可以在蓝图中点击函数或变量旁边的钟形图标，将其设为FieldNotify。
	 * 与蓝图中不同的是，在C++中，你需要手动调用FieldNotify广播。详见https://dev.epicgames.com/documentation/zh-cn/unreal-engine/umg-viewmodel#%E5%9C%A8c++%E4%B8%AD%E5%88%9B%E5%BB%BAviewmodel
	 * 每当你在Gameplay代码中为这些变量设置新值时，它们会向绑定到它们的所有控件发送消息，指示它们更新。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString PlayerName; // 游戏存档名，如"Dungeon Map Saved 1"

	void SetLoadSlotName(FString InLoadSlotName);
	void SetPlayerName(FString InPlayerName);

	FString GetLoadSlotName() const { return LoadSlotName; }
	FString GetPlayerName() const { return PlayerName; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"))
	FString LoadSlotName; // 存档槽位名称，如LoadSlot_0
};
