// CarryChu


#include "UI/WidgetController/AuraWidgetController.h"


/**
 * 设置 Widget 控制器的参数 (初始化依赖项)。
 *
 * @brief 这个函数通常由 HUD 调用。它的作用是将 GAS 系统的核心数据（PlayerController, PlayerState, ASC, AttributeSet）
 * 注入到这个控制器中，方便后续逻辑使用。
 *
 * @param WCParams 包含所有关键数据源的结构体。
 */
void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}


/**
 * 广播初始数值。
 *
 * @brief 这是一个虚函数，旨在由子类（如 UOverlayWidgetController）重写。
 * 当控制器刚刚创建并初始化完毕后，应当调用此函数，将当前的属性值（如满血、满蓝）立刻同步给 UI，
 * 防止 UI 在第一次属性变化前显示为空或默认值。
 */
void UAuraWidgetController::BroadcastInitialValues()
{
	// 基类中不执行任何操作，具体逻辑由子类实现。
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}
