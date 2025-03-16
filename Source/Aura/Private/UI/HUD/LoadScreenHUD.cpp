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
	LoadScreenWidget->SetIsFocusable(true);
	LoadScreenWidget->AddToViewport();

	APlayerController* PC = GetOwningPlayerController();
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(LoadScreenWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);

	//创建完成用户控件后，最后调用用户控件函数
	LoadScreenWidget->BlueprintInitializeWidget();

	// 加载存档数据
	LoadScreenViewModel->LoadData();
}
