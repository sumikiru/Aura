// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved)
		   .AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	// 如果绑定时ASC未初始化成功，则监听ASC创建OnASCRegisteredDelegate，并AddWeakLambda()实现对负面标签的监听
	else if (CombatInterface)
	{
		// 使用AddWeakLambda()而不是AddLambda()，这样的好处是当绑定的对象(InUserObject)被销毁时，委托不会保持对对象的引用，从而避免悬空指针问题和内存泄漏
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(
			this,
			[this](UAbilitySystemComponent* InASC)
			{
				InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved)
				     .AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
			});
	}

	// 绑定：死亡后销毁该NiagaraComponent
	if (CombatInterface)
	{
		// OnDeathDelegate是多播委托，这里采用AddDynamic。注意函数需要使用UFUNCTION()标记
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	const bool bOwnerValid = IsValid(GetOwner());
	const bool bOwnerAlive = bOwnerValid && GetOwner()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(GetOwner());
	
	if (NewCount > 0 && bOwnerAlive)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
