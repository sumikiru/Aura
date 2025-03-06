// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	// 已创建AuraAbilitySystemComponent，则直接绑定Activate事件
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->ActivatePassiveEffectDelegate.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
	}
	// 如果ASC未初始化，通过接口ICombatInterface监听ASC注册事件。确保即使 ASC 未立即创建，也能在其注册后完成事件绑定。
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		// AddWeakLambda这种绑定方式的主要好处是，当绑定的对象被销毁时，委托不会保持对象的引用，从而避免悬空指针问题和内存泄漏。
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(
			this,
			[this](UAbilitySystemComponent* InASC)
			{
				if (UAuraAbilitySystemComponent* InAuraASC = Cast<UAuraAbilitySystemComponent>(InASC))
				{
					InAuraASC->ActivatePassiveEffectDelegate.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				}
			}
		);
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (!AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		return;
	}

	if (bActivate && !IsActive())
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}
