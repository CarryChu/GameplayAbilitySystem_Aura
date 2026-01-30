// CarryChu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

/**
 * 效果应用策略 (枚举)
 * * @brief 定义了 GameplayEffect (GE) 应该在什么时机被应用给目标。
 */
UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
    ApplyOnOverlay,     // 在发生重叠 (BeginOverlap) 时应用效果
    ApplyOnEndOverlay,  // 在结束重叠 (EndOverlap) 时应用效果
    DoNotApply          // 不应用 (通常用于禁用某个插槽)
};

/**
 * 效果移除策略 (枚举)
 * * @brief 定义了当目标离开范围 (EndOverlap) 时，是否需要移除该效果。
 * * 主要用于 "Infinite" (无限) 类型的效果，例如光环。
 */
UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
    RemoveOnOverlay,    // 在结束重叠 (EndOverlap) 时移除效果 (例如离开火堆就不烧了)
    DoNotRemove         // 不移除 (例如踩了陷阱中毒，离开了还得接着毒)
};

/**
 * AAuraEffectActor
 * * @brief 这是一个通用的 GAS 效果触发器 Actor。
 * * 它可以是地上的火坑、拾取的血瓶、或者持续回血的光环区域。
 * * 它的核心逻辑是：监听重叠事件 -> 根据配置的策略 -> 给目标应用 GameplayEffect。
 */
UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
    GENERATED_BODY()
    
public: 
    AAuraEffectActor();

protected:
    virtual void BeginPlay() override;

    /**
     * 应用效果的核心函数
     * * @brief 尝试给目标 Actor 的 AbilitySystemComponent (ASC) 应用指定的 GameplayEffect。
     * * @param TargetActor 受击者/目标。
     * * @param GameplayEffectClass 要应用的 GE 类。
     */
    UFUNCTION(BlueprintCallable)
    void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);
    
    /**
     * 重叠事件处理
     * * @brief 当某个物体进入该 Actor 范围时调用。
     * * 会检查 Instance, Duration, Infinite 三种 GE 的 ApplicationPolicy，
     * * 如果策略是 "ApplyOnOverlay"，则调用 ApplyEffectToTarget。
     */
    UFUNCTION(BlueprintCallable)
    void OnOverlap(AActor* TargetActor);
    
    /**
     * 结束重叠事件处理
     * * @brief 当某个物体离开该 Actor 范围时调用。
     * * 1. 检查 ApplicationPolicy 为 "ApplyOnEndOverlay" 的效果并应用。
     * * 2. 检查 RemovalPolicy，如果是无限效果且需要移除，则在此处移除。
     */
    UFUNCTION(BlueprintCallable)
    void OnEndOverlap(AActor* TargetActor);
    
    // 是否在效果移除后销毁自身 (例如：捡起金币/血瓶后，Actor 应该消失)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
    bool bDestroyOnEffectRemoval = false;
    
    // =================================================================================
    // 1. 即时效果 (Instant Gameplay Effect)
    // =================================================================================
    // 特点：立即生效，不持续。例如：瞬间扣血、瞬间加经验。
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
    TSubclassOf<UGameplayEffect> InstanceGameplayEffectClass;
    
    // 即时效果的应用策略 (通常选 ApplyOnOverlay，即踩上去就触发)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
    EEffectApplicationPolicy InstanceApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

    // =================================================================================
    // 2. 持续效果 (Duration Gameplay Effect)
    // =================================================================================
    // 特点：持续一段时间后自动消失。例如：持续 10 秒的回血 Buff。
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
    TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
    
    // 持续效果的应用策略
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
    EEffectApplicationPolicy DurationApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
    
    // =================================================================================
    // 3. 无限效果 (Infinite Gameplay Effect)
    // =================================================================================
    // 特点：除非手动移除，否则永久存在。例如：站在光环里一直加防御，离开光环防御消失。
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
    TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
    
    // 无限效果的应用策略
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
    EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
    
    // 无限效果的移除策略 (只有无限效果需要这个，因为前两种会自动过期)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
    EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnOverlay;
    
    /**
     * 活跃效果句柄映射表
     * * @brief 用于追踪目前已经应用了的 "无限效果"。
     * * Key: FActiveGameplayEffectHandle (效果的身份证 ID)。
     * * Value: UAbilitySystemComponent (这个效果应用给了谁)。
     * * 为什么要存这个？因为当角色离开光环 (OnEndOverlap) 时，我们需要知道去移除“哪一个”效果。
     */
    UPROPERTY()
    TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
    
    // Actor 等级 (用于 CurveTable 计算伤害或数值，比如 1 级火坑扣 10 血，5 级火坑扣 50 血)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
    float ActorLevel = 1.f;
};