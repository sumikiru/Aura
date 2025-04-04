// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "Characters/AuraEnemy.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AI/AuraAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true; // 具体的RotationRate在蓝图中编辑

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBar->SetupAttachment(GetRootComponent());

	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED); //自定义深度模板值
	GetMesh()->MarkRenderStateDirty(); //强制立即更新
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->MarkRenderStateDirty();

	BaseWalkSpeed = 250.f;
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}
	AuraAIController = Cast<AAuraAIController>(NewController);

	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(BB_HitReactingKey, false);
	const bool bIsRangedAttacker = CharacterClass != ECharacterClass::Warrior;
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(BB_RangedAttackerKey, bIsRangedAttacker);
}

void AAuraEnemy::ReceiveKnockback(const FVector& KnockbackForce)
{
	// AuraAIController只在服务端上有效，所以客户端上敌人受击以后会出现UE崩溃。所以需要if检查，只在服务器设置值
	if (AuraAIController)
	{
		// 需要注意一点：如果是玩家击中敌人，如果当敌人正在执行行为树中的节点MoveTo时，会导致XY的移动失效，从而只有上下的移动
		// 需要在LaunchCharacter之前，停止行为树中的节点MoveTo，该节点必须手动中断
		// Goblin_Spear无法解决，经检查应该将AM_GoblinSpear_HitReact中的HitReact_Goblin_cut中的根运动->启用根运动选项关闭
		AuraAIController->StopMovement();
	}

	Super::ReceiveKnockback(KnockbackForce);
}

void AAuraEnemy::HighlightActor_Implementation()
{
	/**
	 * 记得先加入PostProcessVolume到World中，并且渲染功能->后期处理材质->添加PP_Highlight
	 * 然后到项目设置中搜索custom depth，修改自定义深度-模板通道为"启用模板"
	 */
	GetMesh()->SetRenderCustomDepth(true); //渲染自定义深度通道
	Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	// 不改变OutDestination
}

int32 AAuraEnemy::GetPlayerLevel_Implementation()
{
	return Level;
}

void AAuraEnemy::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);
	if (AuraAIController)
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(BB_DeadKey, true);
	}
	SpawnLoot();
	Super::Die(DeathImpulse);
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation()
{
	return CombatTarget;
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	// AuraAIController只在服务端上有效，所以客户端上敌人受击以后会出现UE崩溃。所以需要if检查，只在服务器设置值
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(BB_HitReactingKey, bHitReacting);
	}
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	// 可选：四处移动而又不至于互相卡住，但效果较差。
	// GetCharacterMovement()->bUseRVOAvoidance = true;
	// GetCharacterMovement()->bOrientRotationToMovement = true;

	InitAbilityActorInfo();
	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}

	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}

	if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnEnemyHealthChanged.Broadcast(Data.NewValue); // 属性改变时广播
			}
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnEnemyMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

		// 为受击标签注册事件
		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact,
		                                                 EGameplayTagEventType::NewOrRemoved)
		                      .AddUObject(
			                      this,
			                      &AAuraEnemy::HitReactTagChanged
		                      );

		// 交给服务器初始化，同步Health和MaxHealth（客户端会设置为0，需要解决网络同步问题）
		if (HasAuthority())
		{
			// BeginPlay初始化时广播，用于初始化ProgressBarFront的百分比
			OnEnemyHealthChanged.Broadcast(AuraAS->GetHealth());
			OnEnemyMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
		}
	}
}

void AAuraEnemy::InitAbilityActorInfo()
{
	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
	OnAscRegisteredDelegate.Broadcast(AbilitySystemComponent);
	// 注册
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved)
	                      .AddUObject(this, &AAuraEnemy::StunTagChanged);
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(
		this,
		CharacterClass,
		Level,
		AbilitySystemComponent
	);
}

void AAuraEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);

	// 修改黑板中Stun的值
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(BB_StunnedKey, bIsStunned);
	}
}
