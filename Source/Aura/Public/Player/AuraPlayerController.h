// CarryChu
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

// 前置声明：告诉编译器这些类存在，但在 .cpp 引用头文件前不知道具体细节
// 这样做是为了加快编译速度，避免头文件循环依赖
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;

/**
 * AAuraPlayerController
 * 玩家控制器类，负责接收玩家的物理输入（按键、鼠标），并指挥 Pawn（角色）行动
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
    virtual void PlayerTick(float DeltaTime) override;
protected:
	// 游戏开始时执行一次，用于初始化数据
	virtual void BeginPlay() override;
    
	// 专门用于绑定输入功能的函数（这是 UE5 的标准流程）
	virtual void SetupInputComponent() override;
    
private:
	// --- 输入资产配置 ---
    
	// 输入映射上下文 (IMC)：比如 "IMC_Default"，包含了一组按键映射
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
    
	// 输入动作 (IA)：比如 "IA_Move"，代表“移动”这个概念，不关心具体是按 W 还是推摇杆
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
    
	// --- 回调函数 ---
    
	// 当 MoveAction 被触发时（比如玩家按住了 W），引擎会自动调用这个函数
	// InputActionValue 包含了具体的输入值（比如 Vector2D: X=1, Y=0）
	void Move(const FInputActionValue& InputActionValue);
	
	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
};