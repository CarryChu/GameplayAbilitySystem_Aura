// CarryChu


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

AAuraPlayerState::AAuraPlayerState()
{
	// --- 关键区别：玩家在 PlayerState 里创建组件 ---
	// OwnerActor = PlayerState
	// AvatarActor = AuraCharacter (后续在 Character 初始化时连接)
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	
	// 必须开启复制，否则客户端无法使用技能
	AbilitySystemComponent->SetIsReplicated(true);
	
	// 创建属性集
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	
	// --- 网络优化 ---
	// 默认 PlayerState 的同步频率很低（因为通常只同步分数）。
	// 但 GAS 需要高频率同步（比如毫秒级的冷却时间、血量变化）。
	// 所以我们要提高同步频率，防止客户端看到的血条卡顿。
	SetNetUpdateFrequency(100.0f); 
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}