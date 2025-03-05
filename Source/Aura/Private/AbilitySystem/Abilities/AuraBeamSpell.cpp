// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	checkf(OwnerCharacter, TEXT("OwnerCharacter is nullptr, check if OwnerCharacter is correctly set in the StoreOwnerVariables() function."));
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (const USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.AddUnique(OwnerCharacter);
			FHitResult HitResult; // 存储SphereTraceSingle找到的结果
			UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter,
			                                        SocketLocation,
			                                        BeamTargetLocation,
			                                        10.f,
			                                        TraceTypeQuery1,
			                                        false,
			                                        ActorsToIgnore,
			                                        EDrawDebugTrace::None,
			                                        HitResult,
			                                        true);
			// 根据HitResult更新MouseHitLocation和MouseHitActor
			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}

	// 为PrimaryTarget绑定死亡事件
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
		{
			// 对应Dynamic Multicast Delegate，需要使用AddDynamic
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(GetAvatarActorFromActorInfo());
	ActorsToIgnore.AddUnique(MouseHitActor);

	TArray<AActor*> OverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		850.f,
		MouseHitActor->GetActorLocation() /* 注意不是MouseHitLocation，因为从斜方向角度看位置一致，但实际上二者位置并不一致 */
	);

	//int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	int32 NumAdditionalTargets = 5;
	UAuraAbilitySystemLibrary::GetClosestTargets(
		NumAdditionalTargets,
		OverlappingActors,
		OutAdditionalTargets,
		MouseHitActor->GetActorLocation());

	// 为AdditionalTarget绑定死亡事件
	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
}

void UAuraBeamSpell::ApplyDamageToSingleTarget(AActor* DamagedTarget, const FGameplayTag& DamageTypeTag)
{
	// 施加伤害时目标可能已经死亡
	if (!IsValid(DamagedTarget))
	{
		return;
	}
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		const FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContext);
		const FGameplayEffectSpecHandle TargetSpecHandle = UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			SpecHandle,
			DamageTypeTag,
			GetDamageAtLevel());

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DamagedTarget))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*TargetSpecHandle.Data);
		}
	}
}

void UAuraBeamSpell::ApplyDamageToAllTargets(AActor* FirstTraceTarget, TArray<AActor*> AdditionalTargets, const FGameplayTag& DamageTypeTag)
{
	ApplyDamageToSingleTarget(FirstTraceTarget, DamageTypeTag);
	for (AActor* Target : AdditionalTargets)
	{
		ApplyDamageToSingleTarget(Target, DamageTypeTag);
	}
}

void UAuraBeamSpell::ApplyDebuffToSingleTarget(AActor* TargetActor)
{
	const FDamageEffectParams& Params = MakeDamageEffectParamsFromClassDefaults(TargetActor);
	if (K2_HasAuthority())
	{
		UAuraAbilitySystemLibrary::ApplyDamageEffect(Params);
	}
}
