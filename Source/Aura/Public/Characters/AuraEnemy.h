// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Characters/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UBehaviorTree;
class UBlackboardComponent;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	virtual void PossessedBy(AController* NewController) override;

	//~ Begin EnemyInterface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	//~ End EnemyInterface

	//~ Begin CombatInterface
	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() override;
	//~ End CombatInterface

	/**
	 * !!!注意：这里委托的名字不能使用OnHealthChanged和OnMaxHealthChanged！！！
	 * 由于AuraEnemy.h包含了头文件OverlayWidgetController.h，而其中重名的OnHealthChanged和OnMaxHealthChanged会导致BeginPlay中设置的BroadCast出现异常
	 * 使得变更后的值无法被正确地广播出去，从而让蓝图UI中显示的Health和MaxHealth的值在初始化阶段始终等于设定的默认值
	 */
	UPROPERTY(BlueprintAssignable) 
	FOnAttributeChangedSignature OnEnemyHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnEnemyMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 2.f;
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	//不需要复制
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	FName BB_HitReactingKey = TEXT("HitReacting");
	UPROPERTY()
	FName BB_DeadKey = TEXT("Dead");
	UPROPERTY()
	FName BB_RangedAttackerKey = TEXT("RangedAttacker");

	/**记得还要在BP_EnemyBase中设置Pawn->AI控制器类为BP_AuraAIController*/
	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController; 
};
