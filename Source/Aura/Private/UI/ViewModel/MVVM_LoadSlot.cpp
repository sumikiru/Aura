// Author sumikiru, made after learning Druid Mechanics' Course.


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::SetLoadSlotName(FString InLoadSlotName)
{
	// 更改变量时，函数需要调用Viewmodel的其中一个通知宏，以将更改广播到绑定的控件。
	// SET_PROPERTY_VALUE 宏会在赋值并广播之前检查值是否已更改。这项检查在为Viewmodel创建Setter函数时很常见
	// 还需要在蓝图中进行视图绑定
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InLoadSlotName);
}

void UMVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlot::SetMapName(FString InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

void UMVVM_LoadSlot::InitializeSlot()
{
	const int32 WidgetSwitcherIndex = static_cast<int32>(SlotStatus);
	SetWidgetSwitcherIndexDelegate.Broadcast(WidgetSwitcherIndex);
}
