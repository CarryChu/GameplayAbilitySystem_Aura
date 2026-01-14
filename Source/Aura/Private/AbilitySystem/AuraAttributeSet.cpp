// CarryChu


#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

/**
 * 构造函数
 * * @brief 初始化属性的默认值。
 * 注意：这些初始值通常只是为了防止数据为空。在实际游戏中，
 * 真正的初始数值通常来源于 GameplayEffect (GE_DefaultAttributes) 或者配置表。
 */
UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(100.f);
	InitMaxMana(100.f);
}

/**
 * 配置网络复制规则
 * * @brief 告诉引擎哪些属性需要从服务器同步到客户端。
 * 这是 UE 多人网络编程的必经之路。
 * * @param OutLifetimeProps 引擎传入的数组，我们需要把要同步的变量加进去。
 */
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// DOREPLIFETIME_CONDITION_NOTIFY: 注册复制属性
	// 1. 类名 (UAuraAttributeSet)
	// 2. 变量名 (Health)
	// 3. 条件 (COND_None): 无条件复制，只要变了就同步。
	// 4. 通知类型 (REPNOTIFY_Always): 只要服务器发来了新数据，客户端就触发 OnRep 函数（哪怕数值碰巧没变）。
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

/**
 * 客户端响应：生命值同步
 * * @brief 当客户端收到服务器发来的新 Health 数值时，自动调用此函数。
 * * @param OldHealth 修改前的旧数值（引擎自动传入，用于比较或回滚）。
 */
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// GAMEPLAYATTRIBUTE_REPNOTIFY 宏
	// 这是 GAS 系统专用的宏。它不仅把新数值赋给变量，
	// 还负责处理“预测系统” (Prediction System) 的回滚逻辑。
	// 必须使用这个宏，而不能简单地手动赋值，否则在网络延迟高时，客户端的预测回滚会出错。
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

/**
 * 客户端响应：最大生命值同步
 */
void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

/**
 * 客户端响应：法力值同步
 */
void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

/**
 * 客户端响应：最大法力值同步
 */
void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}
