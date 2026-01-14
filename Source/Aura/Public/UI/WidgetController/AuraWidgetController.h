// CarryChu

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AuraWidgetController.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;


// --- 结构体封装 ---
// 为什么要写这个结构体？
// 因为每次初始化 Controller 都要传 4 个参数 (PC, PS, ASC, AS) 太麻烦了。
// 打包成一个结构体，以后想加参数（比如加个 InventoryComponent），只需要改这里，不用改函数签名。
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	
	FWidgetControllerParams(){}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS) 
		: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

/**
 * UAuraWidgetController
 * 作用：这是 UI 逻辑的基类（ViewModel）。
 * 它不依赖具体的 Widget，它只负责持有数据源。
 */
UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
	
public:
	// 设置初始参数，把"数据源"存进来
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);
	
	// 虚函数：给子类去实现的。
	// 作用：当 UI 刚打开时，手动广播一次当前数值（防止 UI 显示空的或者 0）
	virtual void BroadcastInitialValues();
	
	virtual void BindCallbacksToDependencies();
protected:
	// --- 核心数据源 ---
	// 子类（比如 OverlayWidgetController）要查血量、蓝量，就从这里拿
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
};
