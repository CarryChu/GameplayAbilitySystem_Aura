// CarryChu


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"


/**
 * 构造函数：初始化组件。
 * 设置网格体 (Mesh) 和检测球体 (Sphere)，并关闭 Tick 以优化性能。
 */
AAuraEffectActor::AAuraEffectActor()
{
	// 性能优化：这个物体不需要每帧运行逻辑，所以关闭 Tick。
	PrimaryActorTick.bCanEverTick = false;

	// 创建可视化的网格体组件
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	
	// 创建用于碰撞检测的球体组件
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(Mesh);
}

/**
 * 重叠开始事件 (OnOverlap)。
 * * @brief 当有物体进入球体范围时触发。
 * 目前用于测试属性修改逻辑：如果进入的是拥有 GAS 系统的角色，就直接修改其血量。
 * * @note 【重要】这里使用了 const_cast 进行强制类型转换，这是一种“临时黑客手段 (Hack)”。
 * 在正规的 GAS 流程中，我们应该创建一个 UGameplayEffect (GE) 并应用给目标，而不是直接修改属性集。
 * 直接修改通常被禁止（因为 GetAttributeSet 返回的是 const 指针），但为了在还没写 GE 之前测试血条 UI，我们先这么做。
 */
void AAuraEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	// 1. 检查进入的 Actor 是否实现了 GAS 接口 (IAbilitySystemInterface)
	// 这样可以过滤掉普通的物体（如石头、墙壁），只对角色生效。
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		// 2. 获取属性集 (AttributeSet)
		// 注意：GetAttributeSet 返回的是 const 指针（只读），意味着原则上不许你直接改数值。
		const UAuraAttributeSet* AuraAttributeSet= Cast<UAuraAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		
		// 3. 【临时代码】强制移除 const 属性 (Mutable)
		// 使用 const_cast 强行把“只读指针”变成“可写指针”。
		// 这在 C++ 中是危险操作，但在此时用于快速测试“加血效果”是可行的。
		// TODO: 未来会替换为 ApplyGameplayEffectToTarget()。
		UAuraAttributeSet* MutableAuraAttributeSet= const_cast<UAuraAttributeSet*>(AuraAttributeSet);
		
		// 4. 修改血量并销毁自己
		// 当前血量 + 25
		MutableAuraAttributeSet->SetHealth(AuraAttributeSet->GetHealth() + 25.f);
		// 当前法力 + 25
		MutableAuraAttributeSet->SetMana(AuraAttributeSet->GetMana() - 25.f);
		
		// 药水被吃掉了，销毁 Actor
		Destroy();
	}
}

/**
 * 重叠结束事件。
 * 目前不需要处理离开逻辑。
 */
void AAuraEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

/**
 * 游戏开始时调用。
 * 用于绑定碰撞委托。
 */
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AAuraEffectActor::EndOverlap);
}



