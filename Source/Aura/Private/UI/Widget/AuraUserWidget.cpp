// CarryChu


#include "UI/Widget/AuraUserWidget.h"


/**
 * 设置依赖项 (Dependency Injection)。
 * 这是 MVVM 架构中的“绑定”步骤：将 UI (View) 与其数据控制器 (ViewModel) 连接起来。
 * @param InWidgetController - 传入的控制器实例，包含了 UI 所需的所有数据源 (ASC, AttributeSet 等)。
 */
void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	// 1. 将传入的控制器保存到本地变量
	// 这样 C++ 层的其他函数或者蓝图后续想要访问数据时，可以直接使用 WidgetController 这个变量。
	WidgetController = InWidgetController;
	
	// 2. 广播/通知蓝图：控制器已就位
	// 这一步至关重要。它会触发蓝图中的 "Event WidgetControllerSet"。
	// 蓝图逻辑应当响应该事件，进行类型转换 (Cast) 并绑定数据更新的委托 (Bind Delegates)。
	WidgetControllerSet();
}
