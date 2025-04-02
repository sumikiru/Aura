// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class IHighlightInterface;
class AMagicCircle;
class UNiagaraSystem;
class UDamageTextComponent;
class USplineComponent;
class UAuraAbilitySystemComponent;
class UAuraInputConfig;
struct FInputActionValue;
class UInputAction;

enum class ETargetingStatus : uint8
{
	TargetingEnemy,
	TargetingNonEnemy,
	NotTargeting
};

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInstance* DecalMaterial = nullptr);
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraInputMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	void Move(const FInputActionValue& InputActionValue);
	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	bool bShiftKeyDown = false;
	void CursorTrace();
	/**
	 * 对于需要进行访问追踪的UPROPERTY成员变量，可以使用TObjPtr<T>来替换裸指针 \n
	 * 对于函数参数、局部指针变量等，则建议使用UObject* 裸指针 \n
	 * TObjPtr作为UPROPERTY成员时可以完全替换裸指针，且在release版本时完全等于裸指针
	 */
	FHitResult CursorHit;
	UPROPERTY()
	TObjectPtr<AActor> LastActor = nullptr;
	UPROPERTY()
	TObjectPtr<AActor> CurrentActor = nullptr;
	static void HighlightActor(AActor* InActor);
	static void UnHighlightActor(AActor* InActor);

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	/* 缓存目的地 */
	FVector CachedDestination = FVector::ZeroVector;
	/* 追踪鼠标按下的时间 */
	float FollowTime = 0.f;
	/** 短按时间阈值，超过以后才算长按 */
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	/* 用于确定按下鼠标左键时，是要进行移动还是朝目标使用技能 */
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;

	/** 进入目标半径范围后停下 */
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 5.f;
	/* 使移动路线更加平滑 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	void AutoRun();
	void UpdatePathPoints();

	FTimerHandle UpdatePathPointsTimerHandle;
	/*更新频率越高，则路线更加精确，但是开销更大*/
	float UpdatePathPointsTimeRate = 0.2f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;
	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;

	void UpdateMagicCircleLocation();
};
