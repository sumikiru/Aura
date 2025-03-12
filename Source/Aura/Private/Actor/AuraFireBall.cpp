// Author sumikiru, made after learning Druid Mechanics' Course.


#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "GameplayCueManager.h"


void AAuraFireBall::Blast()
{
	OnHit();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	ActorsToIgnore.Add(this);
	TArray<AActor*> OverlappingEnemies;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		this,
		OverlappingEnemies,
		ActorsToIgnore,
		RadialDamageOuterRadius,
		GetActorLocation());

	UAuraAbilitySystemLibrary::SetRadialDamageEffectParam(
		ExplosionDamageParams,
		true,
		RadialDamageInnerRadius,
		RadialDamageOuterRadius,
		GetActorLocation());

	for (AActor* TargetActor : OverlappingEnemies)
	{
		// 设置ExplosionDamageParams: 其中TargetAbilitySystemComponent为必需
		UAuraAbilitySystemLibrary::SetTargetEffectParamsASC(
			ExplosionDamageParams,
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
		const FVector& Direction = TargetActor->GetActorLocation() - GetActorLocation();
		const FVector& ImpulseDirection = Direction.RotateAngleAxis(45.f, GetActorForwardVector());
		UAuraAbilitySystemLibrary::SetKnockbackDirection(ExplosionDamageParams, ImpulseDirection, KnockbackMagnitude);
		UAuraAbilitySystemLibrary::SetDeathImpulseDirection(ExplosionDamageParams, ImpulseDirection, DeathImpulseMagnitude);
		// 造成伤害：MakeDamageEffectParamsFromClassDefaults()在SpawnFireBalls中
		UAuraAbilitySystemLibrary::ApplyDamageEffect(ExplosionDamageParams);
	}

	Destroy();
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();

	StartOutgoingTimeline();
}

void AAuraFireBall::OnHit()
{
	// GameplayCue默认复制，这里用C++实现仅在本地执行，减小开销
	if (GetOwner())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FAuraGameplayTags::Get().GameplayCue_FireBlast, CueParams);
	}

	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}

void AAuraFireBall::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}

	//服务器进行处理
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// 设置DamageEffectParams
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;

			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			// 应用DamageEffect
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
