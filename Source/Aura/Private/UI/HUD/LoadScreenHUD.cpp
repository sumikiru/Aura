// Author sumikiru, made after learning Druid Mechanics' Course.


#include "UI/HUD/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	//实例化MVVM
	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	LoadScreenViewModel->InitializeLoadSlots();

	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	/**
	 * Note that this property is only set at construction and is not modifiable at runtime.
	 * 错误，需要在编译阶段设置：LoadScreenWidget->SetIsFocusable(true);
	 * 详见ULoadScreenWidget::NativeConstruct()
	 * 或者直接在蓝图中设置WBP_LoadScreen->类默认值->可聚焦改为true
	 */
	LoadScreenWidget->AddToViewport();

	APlayerController* PC = GetOwningPlayerController();
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(LoadScreenWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// 可能出现报错：Error: InputMode:UIOnly - Attempting to focus Non-Focusable widget SObjectWidget
	// 需要在WBP_LoadScreen->类默认值->可聚焦改为true
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);

	//创建完成用户控件后，最后调用用户控件函数
	LoadScreenWidget->BlueprintInitializeWidget();

	// 加载存档数据
	LoadScreenViewModel->LoadData();
}
