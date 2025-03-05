// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();
	/**
	 * 追踪闪电链命中的第一个目标
	 * @param BeamTargetLocation 鼠标点击目标位置
	 * @note 有可能会被中间其他的Actor阻挡，导致追踪的目标不是鼠标选中的目标
	 */
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);
	/**
	 * 通过技能命中目标获取扩散的敌人目标（不包括First Target）
	 * @param OutAdditionalTargets 待修改的最近目标数组
	 */
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);
	UFUNCTION(BlueprintCallable)
	void ApplyDamageToSingleTarget(AActor* DamagedTarget, const FGameplayTag& DamageTypeTag);
	UFUNCTION(BlueprintCallable)
	void ApplyDamageToAllTargets(AActor* FirstTraceTarget, TArray<AActor*> AdditionalTargets, const FGameplayTag& DamageTypeTag);
	UFUNCTION(BlueprintCallable)
	void ApplyDebuffToSingleTarget(AActor* TargetActor);
	/**
	 * 鼠标命中的敌人（初始目标）死亡处理
	 * @param DeadActor 死亡的初始目标
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);
	/**
	 * 鼠标命中敌人的附加敌人（与初始目标相邻）死亡处理
	 * @param DeadActor 鼠标命中敌人的附加敌人
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;
	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	int32 MaxNumShockTargets = 5; // 最多可以攻击的敌人数量
};
