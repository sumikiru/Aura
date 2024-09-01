// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "Characters/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraEnemy::HighlightActor()
{
	/**
	 * 记得先加入PostProcessVolume到World中，并且渲染功能->后期处理材质->添加PP_Highlight
	 * 然后到项目设置中搜索custom depth，修改自定义深度-模板通道为"启用模板"
	 */
	GetMesh()->SetRenderCustomDepth(true);//渲染自定义深度通道
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);//自定义深度模板值
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	check(AbilitySystemComponent);
	
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}
