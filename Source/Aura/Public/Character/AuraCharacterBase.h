// CarryChu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h" // 引入接口，表明这个类能与 GAS 交互
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

// 前向声明，避免在此处引用头文件，加快编译速度
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS(Abstract) // Abstract 表示这个类不能直接拖入场景，必须继承它（比如创建 Hero 或 Enemy）
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	
	// --- IAbilitySystemInterface 接口实现 ---
	// 这是 GAS 系统寻找组件的标准入口。
	// 不管 ASC 存在哪里（身上还是 PlayerState），外界都调用这个函数来获取它。
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// 获取属性集（生命值、法力值等）的 Getter
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };

protected:
	virtual void BeginPlay() override;

	// UPROPERTY 宏：让下面的变量能被虚幻引擎的反射系统识别
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	
	// --- GAS 组件指针 ---
	// 注意：这里只是指针！
	// 对于 Enemy：它指向 Enemy 自己创建的组件。
	// 对于 Player：它指向 PlayerState 里创建的组件（作为引用/缓存）。
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};