// CarryChu

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"

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
    InitMana(10.f);
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
 * 属性变更预处理
 * * @brief 在属性值真正被修改**之前**触发。
 * * 主要用途：限制（Clamp）属性的“当前值”（Current Value）。
 * * 注意：这个函数只影响本次查询/计算的返回值，不会永久修改属性的 Base Value。
 * 例如：你有一个 buff 增加了 200 血量，这里会将其限制在 MaxHealth，但不会改变那个 buff 的数值。
 */
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    
    // 如果即将改变的是生命值
    if (Attribute == GetHealthAttribute())
    {
       // 将新值限制在 [0, MaxHealth] 之间
       NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    
    // 如果即将改变的是法力值
    if (Attribute == GetManaAttribute())
    {
       // 将新值限制在 [0, MaxMana] 之间
       NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
    }
}

/**
 * 辅助函数：填充上下文信息
 * * @brief 从 GameplayEffect 的回调数据中提取 Source（施法者）和 Target（受击者）的详细信息。
 * * 这是一个非常标准的 GAS 工具函数，用于将复杂的 Data 结构解构成易用的 Props 结构体。
 */
void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
    // 1. 获取 EffectContextHandle (包含了施法时的所有上下文信息，如位置、物体等)
    Props.EffectContextHandle = Data.EffectSpec.GetContext();
    
    // 2. 获取施法者 (Source) 的 ASC (Ability System Component)
    Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
    
    // 3. 如果施法者 ASC 存在，且其 ActorInfo 有效
    if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
    {
       // 获取施法者的 AvatarActor (通常是 Character 或 Pawn)
       Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
       
       // 获取施法者的 PlayerController
       Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
       
       // 特殊情况处理：如果 ASC 中没有缓存 Controller，但 AvatarActor 存在
       // (例如：AI 控制的 Pawn，或者某些时刻 Controller 还没注册到 ASC)
       if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
       {
          if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
          {
             // 尝试直接从 Pawn 身上获取 Controller
             Props.SourceController = Pawn->GetController();
          }
       }
       
       // 如果最终找到了 Controller，尝试获取它控制的 Character
       if (Props.SourceController)
       {
          // 这里使用局部变量，并未存入 Props，如果 Props 结构体里有 SourceCharacter 字段，建议赋值
          ACharacter* SourceCharacter = Cast<ACharacter>(Props.SourceController->GetCharacter());
       }
    }
    
    // 4. 获取受击者 (Target) 的信息
    // Data.Target 是受击者的 ASC 引用
    if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
    {
       Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
       
       Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
       
       Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
    
       // 获取受击者的 ASC 指针
       Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
    }
}

/**
 * 属性修改后回调
 * * @brief 当 GameplayEffect 成功应用并修改了属性的 Base Value **之后**触发。
 * * 用途：处理游戏核心逻辑。
 * * 例如：扣血后检查是否死亡、显示飘字伤害、触发受击特效等。
 */
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // 准备一个结构体来存放上下文信息
    FEffectProperties Props;
    // 调用上面的辅助函数填充 Props，这样后面就可以直接用 Props.SourceCharacter 等变量了
    SetEffectProperties(Data, Props);
    
    // TODO: 在这里添加游戏逻辑
    // 比如：
    // if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    // {
    //      SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth())); // 再次 Clamp 以确保 Base Value 也是安全的
    //      if (GetHealth() <= 0) { // 处理死亡逻辑 }
    // }
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