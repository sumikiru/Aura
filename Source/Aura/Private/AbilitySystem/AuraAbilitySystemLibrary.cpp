// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "Engine/OverlapResult.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWidgetControllerParams,
                                                           AAuraHUD*& OutAuraHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutAuraHUD = Cast<AAuraHUD>(PC->GetHUD());
		if (OutAuraHUD)
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			OutWidgetControllerParams.PlayerController = PC;
			OutWidgetControllerParams.PlayerState = PS;
			OutWidgetControllerParams.AbilitySystemComponent = ASC;
			OutWidgetControllerParams.AttributeSet = AS;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level,
                                                            UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr)
	{
		return;
	}
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	AActor* AvatarActor = ASC->GetAvatarActor();

	//ApplyGameplayEffectSpecToSelf
	// Note:需要AddSourceObject
	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(
		ClassDefaultInfo.PrimaryAttributes,
		Level,
		PrimaryAttributesContextHandle
	);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(
		CharacterClassInfo->SecondaryAttributes,
		Level,
		SecondaryAttributesContextHandle
	);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(
		CharacterClassInfo->VitalAttributes,
		Level,
		VitalAttributesContextHandle
	);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr)
	{
		return;
	}
	// Common Abilities
	for (const TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	// Startup Abilities
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject,
                                                             ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr)
	{
		return 0;
	}

	const FCharacterClassDefaultInfo Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);
	return static_cast<int32>(XPReward);
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr)
	{
		return nullptr;
	}

	return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr)
	{
		return nullptr;
	}

	return AuraGameMode->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->IsCriticalHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (AuraGameplayEffectContext->GetDamageType().IsValid())
		{
			return *AuraGameplayEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->IsRadialDamage();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedHit)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsSuccessfulDebuff(bInIsSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, const float InDebuffDamage)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, const float InDebuffFrequency)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, const float InDebuffDuration)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AuraGameplayEffectContext->SetDamageType(DamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetDeathImpulse(InDeathImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockbackForce)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetKnockbackForce(InKnockbackForce);
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsRadialDamage)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle, const float InRadialDamageInnerRadius)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetRadialDamageInnerRadius(InRadialDamageInnerRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle, const float InRadialDamageOuterRadius)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetRadialDamageOuterRadius(InRadialDamageOuterRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InRadialDamageOrigin)
{
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetRadialDamageOrigin(InRadialDamageOrigin);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors,
                                                           const TArray<AActor*>& ActorsToIgnore, const float Radius,
                                                           const FVector& SphereOriginLocation)
{
	// 参照UGameplayStatics::ApplyRadialDamageWithFalloff
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		// 查询场景，查看hit结果
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOriginLocation, FQuat::Identity,
		                                FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
		                                FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			// 只是检查Actor是否实现该CombatInterface接口，所以可以使用UCombatInterface（反射说明符MinimalAPI: This class does not need to be modified.）
			// 同时定义两个bool常量无法实现短路判断
			// const bool bImplementsCombatInterface = Overlap.GetActor()->Implements<UCombatInterface>();
			// const bool bIsAlive = !ICombatInterface::Execute_IsDead(Overlap.GetActor());
			if (Overlap.GetActor()->Implements<UCombatInterface>() &&
				!ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor())); //直接传入Overlap.GetActor()也可以
			}
		}
	}
}

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets,
                                                  const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	// 原方法--------------------------------------------------
	/*TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;

	while (NumTargetsFound < MaxTargets)
	{
		if (ActorsToCheck.Num() == 0)
		{
			break;
		}
		double ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor;
		for (AActor* PotentialTarget : ActorsToCheck)
		{
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}
		ActorsToCheck.Remove(ClosestActor);
		OutClosestTargets.AddUnique(ClosestActor);
		NumTargetsFound++;
	}*/

	// 使用优先队列代替------------------------------------------
	TArray<AActor*> ActorsToCheck = Actors;
	TArray<TPair<double, AActor*>> DistanceHeap;
	// 预分配内存提升性能
	DistanceHeap.Reserve(ActorsToCheck.Num());
	// 构建距离-对象映射
	for (AActor* Actor : ActorsToCheck)
	{
		const double Distance = (Actor->GetActorLocation() - Origin).Length();
		DistanceHeap.Emplace(Distance, Actor);
	}
	// 自定义堆排序谓词（最小堆）
	auto MinHeapPredicate = [](const TPair<double, AActor*>& A, const TPair<double, AActor*>& B)
	{
		return A.Key < B.Key; // 距离更小的优先级更高
	};
	// 将数组转换为最小堆
	DistanceHeap.Heapify(MinHeapPredicate);
	// 提取最近目标
	int32 NumTargetsFound = 0;
	while (!DistanceHeap.IsEmpty() && NumTargetsFound < MaxTargets)
	{
		// 弹出堆顶最小元素
		TPair<double, AActor*> ClosestPair;
		DistanceHeap.HeapPop(ClosestPair, MinHeapPredicate);
		if (ClosestPair.Value && !OutClosestTargets.Contains(ClosestPair.Value))
		{
			OutClosestTargets.AddUnique(ClosestPair.Value);
			NumTargetsFound++;
		}
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	if (FirstActor->ActorHasTag(FName("Player")))
	{
		return !SecondActor->ActorHasTag(FName("Player"));
	}
	if (FirstActor->ActorHasTag(FName("Enemy")))
	{
		return !SecondActor->ActorHasTag(FName("Enemy"));
	}
	return false;
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, const float SpreadAngleDeg,
                                                                 const int32 NumRotators)
{
	TArray<FRotator> Rotators;

	const FVector LeftSpreadBoundary = Forward.RotateAngleAxis(-SpreadAngleDeg / 2.f, Axis);
	if (NumRotators > 1)
	{
		const float DeltaSpreadDegree = SpreadAngleDeg / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftSpreadBoundary.RotateAngleAxis(DeltaSpreadDegree * i, Axis);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}

	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, const float SpreadAngleDeg,
                                                                const int32 NumVectors)
{
	TArray<FVector> Vectors;

	const FVector LeftSpreadBoundary = Forward.RotateAngleAxis(-SpreadAngleDeg / 2.f, Axis);
	if (NumVectors > 1)
	{
		const float DeltaSpreadDegree = SpreadAngleDeg / (NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftSpreadBoundary.RotateAngleAxis(DeltaSpreadDegree * i, Axis);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}

	return Vectors;
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	// 创建GE的上下文句柄
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, DamageEffectParams.KnockbackForce);
	SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);

	// 根据句柄和类创建GE实例
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->
	                                                                MakeOutgoingSpec(DamageEffectParams.DamageEffectClass,
	                                                                                 DamageEffectParams.AbilityLevel,
	                                                                                 EffectContextHandle);

	// 根据标签设置GE使用的配置
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);

	// 将GE应用给目标ASC
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContextHandle;
}
