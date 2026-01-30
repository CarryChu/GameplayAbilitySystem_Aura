// CarryChu

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

// ====================================================================================
// GAS 核心宏定义
// ====================================================================================
/**
 * ATTRIBUTE_ACCESSORS 宏
 * * @brief 这是一个极其常用的宏，用于自动生成属性的样板代码 (Boilerplate Code)。
 * * 如果没有这个宏，你需要手动为每一个属性写 Getter, Setter, Init 函数。
 * * 它会自动生成以下函数 (以 Health 为例):
 * 1. GetHealth(): 返回当前 Health 值。
 * 2. SetHealth(float NewVal): 设置 Health 值。
 * 3. InitHealth(float NewVal): 初始化 Health 值。
 * 4. GetHealthAttribute(): 返回 FGameplayAttribute 结构体 (用于反射和系统查找)。
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 上下文辅助结构体
 * * @brief 这是一个自定义的辅助结构体 (非 GAS 内置，但非常实用)。
 * * 用途：用于在 PostGameplayEffectExecute 中整理数据。
 * * GAS 的原始回调数据 (FGameplayEffectModCallbackData) 嵌套很深，
 * * 我们用这个结构体把 "施法者(Source)" 和 "受击者(Target)" 的关键信息（ASC, Actor, Controller, Character）
 * * 提取出来存好，方便后续逻辑使用。
 */
USTRUCT()
struct FEffectProperties
{
    GENERATED_BODY()
    
    FEffectProperties(){}
    
    // GameplayEffect 的上下文句柄 (包含命中位置、物体等信息)
    FGameplayEffectContextHandle EffectContextHandle;
    
    // === 施法者 (Source) 信息 ===
    // 谁发起了这个效果？(例如：攻击者)
    
    UPROPERTY()
    UAbilitySystemComponent* SourceASC = nullptr;
    
    UPROPERTY()
    AActor* SourceAvatarActor = nullptr;
    
    UPROPERTY()
    AController* SourceController = nullptr;
    
    UPROPERTY()
    ACharacter* SourceCharacter = nullptr;
    
    // === 受击者 (Target) 信息 ===
    // 谁拥有这个 AttributeSet？(例如：被攻击者/玩家自己)
    
    UPROPERTY()
    UAbilitySystemComponent* TargetASC = nullptr;
    
    UPROPERTY()
    AActor* TargetAvatarActor = nullptr;
    
    UPROPERTY()
    AController* TargetController = nullptr;
    
    UPROPERTY()
    ACharacter* TargetCharacter = nullptr;
};

/**
 * UAuraAttributeSet
 * * @brief 属性集类。这是 GAS 系统中存储数值（血量、法力、攻击力等）的核心容器。
 * * 任何拥有 AbilitySystemComponent 的角色，都需要关联一个 AttributeSet 才能拥有属性。
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
public:
    UAuraAttributeSet();
    
    /**
     * 网络复制规则
     * * @brief 必须重写此函数以注册需要从服务器同步到客户端的变量。
     */
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    
    /**
     * 属性变更预处理 (Pre-Change)
     * * @brief 在属性值被修改**之前**调用。
     * * 这里的修改通常指的是查询时的临时修改 (CurrentValue)，不会永久改变 BaseValue。
     * * 典型用途：限制数值范围 (Clamp)，例如保持血量在 0 到 MaxHealth 之间。
     */
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    
    /**
     * Effect 执行后回调 (Post-Execute)
     * * @brief 当一个 Instant GameplayEffect (即时效果) 成功修改了属性**之后**调用。
     * * 这是处理游戏核心逻辑的地方（例如：扣血后判断死亡、受伤反应、跳字）。
     * * 只有 Instant 类型的 GE 会触发此函数 (Duration 和 Infinite 类型不会触发)。
     */
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    
    // ====================================================================================
    // 属性定义区
    // ====================================================================================
    
    /* * ReplicatedUsing = OnRep_Health: 指定当客户端收到服务器更新时，调用 OnRep_Health 函数。
     * FGameplayAttributeData: 这是一个结构体，包含 BaseValue (基础值) 和 CurrentValue (当前值)。
     */
    
    // === 生命值 (Health) ===
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health ,Category="Vital Attributes")
    FGameplayAttributeData Health;
    // 使用宏生成 Getter/Setter/Init
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
    
    // === 最大生命值 (MaxHealth) ===
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth ,Category="Vital Attributes")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
    
    // === 法力值 (Mana) ===
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana ,Category="Vital Attributes")
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);
    
    // === 最大法力值 (MaxMana) ===
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana ,Category="Vital Attributes")
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);
    
    // ====================================================================================
    // 网络同步回调函数 (RepNotifies)
    // ====================================================================================
    
    /**
     * @brief 客户端收到 Health 更新时的回调。
     * 必须在内部调用 GAMEPLAYATTRIBUTE_REPNOTIFY 宏来处理预测回滚。
     */
    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
    
    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
    
    UFUNCTION()
    void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
    
    UFUNCTION()
    void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
    
private:
    
    /**
     * @brief 这是一个私有辅助函数，负责填充 FEffectProperties 结构体。
     * 仅在 PostGameplayEffectExecute 内部使用，用于简化代码，避免主函数过于臃肿。
     */
    void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};