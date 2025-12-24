// CarryChu


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"


AAuraEnemy::AAuraEnemy()
{
	// 敌人的碰撞设置，确保鼠标能点击到它（用于后续的选中逻辑）
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	// --- 关键区别：AI 直接在自己身上创建组件 ---
	// 敌人不需要重生，也不需要跨关卡保存状态。
	// 所以 OwnerActor 和 AvatarActor 都是敌人自己。
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	// 开启网络复制，确保多人游戏中服务器的技能状态能同步给客户端
	AbilitySystemComponent->SetIsReplicated(true);
	
	// 创建属性集
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}
