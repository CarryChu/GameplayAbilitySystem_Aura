// CarryChu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * PlayerState 用于存储玩家的“非物理”数据（分数、名字、RPG属性）。
 * 当玩家角色死亡（Character被销毁）时，PlayerState 依然存在。
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	
	// 实现接口：如果外界问 PlayerState 要 ASC，我们也能给出来
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };
	
protected:
	// 这里是 ASC 真正的“家”（对于玩家而言）
	// 内存是在这里分配的
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};