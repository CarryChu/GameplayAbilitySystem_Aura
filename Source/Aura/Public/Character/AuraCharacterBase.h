// CarryChu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

protected:
	virtual void BeginPlay() override;

	// UPROPERTY 宏：让下面的变量能被虚幻引擎的反射系统识别
	// EditAnywhere: 允许在“蓝图编辑器”和“场景实例”中修改这个属性
	// Category: 在细节面板里，把它归类到 "Combat"（战斗）这个折叠栏下，方便查找
	UPROPERTY(EditAnywhere, Category = "Combat")

	// 声明一个“骨骼网格体组件”指针，名字叫 Weapon
	// TObjectPtr: UE5 推荐的智能指针写法，用于优化编辑器加载性能
	TObjectPtr<USkeletalMeshComponent> Weapon;

};
