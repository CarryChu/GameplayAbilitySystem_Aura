// CarryChu


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"


/**
 * 广播初始数值。
 * * @brief 当 OverlayWidgetController 被创建并设置好参数后，此函数会被立即调用。
 * 它的作用是主动“推”一次数据给 UI，确保游戏刚开始时，血条和蓝条显示的是当前属性值（例如满血），
 * 而不是空的或者默认值。
 */
void UOverlayWidgetController::BroadcastInitialValues()
{
	// 1. 获取具体的属性集 (Downcast)
	// 基类 (AuraWidgetController) 只持有通用的 UAttributeSet 指针。
	// 这里我们需要把它强转为项目专用的 UAuraAttributeSet，以便访问具体的 GetHealth(), GetMana() 等函数。
	// CastChecked: 如果转换失败（比如类型不对），游戏会直接崩溃报错，方便我们在开发阶段发现 Bug。
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	// 2. 广播当前属性值
	// 此时游戏刚开始，还没有任何“变化”发生，所以我们需要手动调用 Broadcast。
	// 这样 UI 里的 BindEvent 就会收到消息，把进度条更新到正确的位置。
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
