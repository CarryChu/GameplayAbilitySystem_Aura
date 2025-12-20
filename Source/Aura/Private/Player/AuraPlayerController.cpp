// CarryChu
#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h" // 必须包含：用于获取增强输入子系统
#include "EnhancedInputComponent.h" // 必须包含：用于绑定输入动作

AAuraPlayerController::AAuraPlayerController()
{
    // 开启网络复制：如果是多人游戏，这个控制器需要同步数据
    bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // 断言检查：如果忘记在蓝图中设置 AuraContext，游戏会直接崩溃报错，提醒你通过
    check(AuraContext);
    
    // 1. 获取增强输入子系统 (Subsystem)
    // 这是一个单例模式，依附于 LocalPlayer 存在
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    
    // 2. 将我们的按键映射 (IMC) 添加到子系统中
    // Priority 0: 优先级。如果有多个 IMC，优先级高的会覆盖低的
    if (Subsystem) // 最好加个 if 判断，虽然 check 也能拦截
    {
        Subsystem->AddMappingContext(AuraContext, 0);
    }
    
    // --- 鼠标光标设置 (RPG 风格) ---
    bShowMouseCursor = true; // 显示鼠标光标
    DefaultMouseCursor = EMouseCursor::Default; // 设置光标样式为默认箭头
    
    // 设置输入模式：GameAndUI
    // 允许玩家同时控制游戏角色和点击 UI 界面
    FInputModeGameAndUI InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 鼠标不锁定在窗口内，可以移出
    InputModeData.SetHideCursorDuringCapture(false); // 点击时不要隐藏鼠标
    SetInputMode(InputModeData); // 应用设置
}

void AAuraPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    // 1. 强制类型转换：把基础的 InputComponent 转为增强版 UEnhancedInputComponent
    // CastChecked: 如果转换失败（比如项目没启用增强输入），游戏会直接崩溃
    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
    
    // 2. 绑定动作：将 "MoveAction" 资产连接到 "Move" 函数
    // ETriggerEvent::Triggered: 只要按键按着，每帧都会调用 Move 函数（适合持续移动）
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
    // 1. 获取输入的二维向量 (X, Y)
    // 对于 WASD：W=(0,1), S=(0,-1), A=(-1,0), D=(1,0)
    const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
    
    // --- 核心数学逻辑：计算移动方向 ---
    
    // 2. 获取控制器当前的旋转角度 (通常等于摄像机的角度)
    const FRotator Rotator = GetControlRotation();
    
    // 3. 提取 Yaw (偏航角/水平旋转)，并忽略 Pitch (俯仰) 和 Roll (翻滚)
    // 为什么？因为我们只希望角色在地面上水平移动，不希望他向着天空飞或钻地
    const FRotator YawRotator(0.f, Rotator.Yaw, 0.f);

    // 4. 计算 "前进方向" (Forward) 和 "右侧方向" (Right)
    // FRotationMatrix(YawRotator): 创建一个基于当前 Yaw 角的旋转矩阵
    // GetUnitAxis(EAxis::X): 获取矩阵的 X 轴方向（在 UE 里，X 轴就是前方）
    // GetUnitAxis(EAxis::Y): 获取矩阵的 Y 轴方向（在 UE 里，Y 轴就是右方）
    const FVector ForwardDirection = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);
    
    // 5. 施加移动输入
    if (APawn* ControlledPawn = GetPawn<APawn>())
    {
       // 向前/后移动：使用 InputAxisVector.Y (W/S键的值)
       ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
       
       // 向左/右移动：使用 InputAxisVector.X (A/D键的值)
       ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
    }
}