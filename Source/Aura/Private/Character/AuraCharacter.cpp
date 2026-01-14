// CarryChu


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
}

// 服务器端逻辑：当控制器附身到角色时调用
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// 服务器需要初始化 GAS 信息
	InitAbilityActorInfo();
}

// 客户端逻辑：当 PlayerState 同步到客户端时调用
void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	// 客户端也需要初始化 GAS 信息
	InitAbilityActorInfo();
}

// --- 初始化核心 ---
void AAuraCharacter::InitAbilityActorInfo()
{
	// 1. 获取 PlayerState (因为对于主角，ASC 存在 PlayerState 里)
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	
	// 2. 初始化 GAS 的 Actor 信息
	// Owner (拥有者) = PlayerState (负责逻辑和数据)
	// Avatar (化身) = This Character (负责表现和动作)
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState,this);
	
	// 3. 把组件指针缓存到 Character 身上，方便以后用
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	
	// 4. 【初始化 UI】
	// 只有本地玩家才需要 UI (多人游戏里你不需要看到队友的屏幕 UI)
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		// 只有本地玩家控制器才有 HUD
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			// HUD：所有的需要的数据 (PC, PS, ASC, AS) 都齐了，开始组装 UI
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}
