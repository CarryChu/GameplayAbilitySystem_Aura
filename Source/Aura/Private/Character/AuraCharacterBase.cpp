// CarryChu


#include "Character/AuraCharacterBase.h"

// 构造函数：当这个类被创建时（或者游戏启动创建默认对象 CDO 时）执行
AAuraCharacterBase::AAuraCharacterBase()
{
	// 性能优化设置：禁止每帧更新 (Tick)
	// 默认是 true。如果不设置为 false，引擎每秒会调用 Tick 函数 60 次（假设 60 帧）
	// 大多数角色不需要每帧都在 C++ 里跑逻辑，关掉它可以节省 CPU 性能
	PrimaryActorTick.bCanEverTick = false;

	// 1. 创建组件：在内存中实例化一个“骨骼网格体组件”
	// 这里的 "Weapon" 字符串是它在编辑器“组件列表”里显示的名字
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");

	// 2. 绑定组件：把这个武器“粘”到角色的身体上
	// GetMesh(): 获取角色的主网格体（比如那个法师模型）
	// FName("WeaponHandSocket"): 指定粘在“右手”的插槽上（这个名字必须和美术在骨骼里设置的名字一模一样）
	// 这样武器就会跟着手一起动
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	
	// 3. 设置碰撞：关闭武器的物理碰撞
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

