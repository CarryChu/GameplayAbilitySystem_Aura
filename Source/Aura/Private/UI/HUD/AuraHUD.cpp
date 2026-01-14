// CarryChu


#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

// 单例模式 (Singleton-like) 的获取器
// 作用：确保对于同一个 HUD，只创建一个 OverlayWidgetController。
// 如果已经创建过了，就直接返回旧的，别浪费内存。
UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
		
		return OverlayWidgetController;
	}
	
	return OverlayWidgetController;
}

// --- 核心初始化函数 ---
// 这个函数通常由 Character 在 BeginPlay/PossessedBy 时调用
void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	// 1. 安全检查：防止你在蓝图里忘了选类，导致游戏崩溃
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized ,please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized ,please fill out BP_AuraHUD"));
	
	// 2. 创建 UI (View)
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	
	OverlayWidget = Cast<UAuraUserWidget>(Widget);
	OverlayWidget->SetWidgetController(Widget);
	
	// 3. 准备数据包 (Model/Data)
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	
	// 5. 【关键】连接 UI 和 控制器
	// 这一步会触发 Widget 蓝图里的 "WidgetControllerSet" 事件，让你去绑定进度条
	OverlayWidget->SetWidgetController(WidgetController);
	
	// 6. 手动广播一次初始值 (防止UI刚出来是空的)
	WidgetController->BroadcastInitialValues();
	// 7. 显示在屏幕上
	Widget->AddToViewport();
}

