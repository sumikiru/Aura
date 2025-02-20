// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AuraLogChannels.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	/**
	 * multicast delegate
	 * only called on a server, executed on all clients
	 */
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag); // 手动添加InputTag
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped); // 标记为已装备技能
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec); //无需玩家输入的一次性技能（如被动技能的初始化逻辑）
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	/**
	 * 当遍历 ActivatableAbilities.Items 这样的 Ability 列表时，
	 * 需要在循环上方添加 ABILITYLIST_SCOPE_LOCK () 宏来锁定列表（上锁），防止在遍历过程中意外删除/修改 Ability 导致的问题。
	 * 遍历结束后解锁，保证线程安全
	 */
	FScopedAbilityListLock ActiveScopeLock(*this); /** 增删并等待该Scope完成后继续 */
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			// 只需要包含Abilities名称
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		// UAuraAbilitySystemComponent::AddCharacterAbilities中添加StartupInputTag:
		// AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
		// 所以应该遍历AbilitySpec.DynamicAbilityTags而不是AbilitySpec.Ability.Get()->AbilityTags
		// 区别在于。InputTag将标签存储在AbilitySpec.DynamicAbilityTags中，而AbilityTag将标签存储在AbilitySpec.Ability.Get()->AbilityTags中（GA蓝图中手动分配）
		for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags) //从技能实例的动态标签容器中遍历所有标签
		{
			// 只需要包含InputTag名称
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*Spec); // 调用了成员函数，所以不能声明为static
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec); // 调用了成员函数，所以不能声明为static
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		/*for (FGameplayTag Tag : AbilitySpec.Ability->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}*/
		// 以下为简化版
		if (AbilitySpec.Ability->AbilityTags.HasTag(AbilityTag))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (IsValid(GetAvatarActor()) && GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (IsValid(GetAvatarActor()) && GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
		}
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid() || Level < Info.LevelRequirement || GetSpecFromAbilityTag(Info.AbilityTag) != nullptr)
		{
			continue;
		}
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
		// 标记能力状态符合资格
		AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
		GiveAbility(AbilitySpec); // 暂不激活
		// 强制立即复制AbilitySpec
		MarkAbilitySpecDirty(AbilitySpec);

		ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		// Spend Point
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		// 更新技能状态（可能升级等级）
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			// 将状态标签从Eligible更换为Unlocked
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
			Status = GameplayTags.Abilities_Status_Unlocked;
		}
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			// 提升技能等级，注意此时如果能力处于活动状态，需要删除该能力然后再恢复
			AbilitySpec->Level += 1;
		}

		// 注意记得Broadcast
		ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec); // 强制立即更新
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		const FGameplayTag& PreviousSlot = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);

		const bool bStatusValid = Status == FAuraGameplayTags::Get().Abilities_Status_Equipped ||
			Status == FAuraGameplayTags::Get().Abilities_Status_Unlocked;
		if (bStatusValid)
		{
			// Remove this InputTag(slot) from any abilities that has it.
			ClearAbilitiesOfSlot(Slot);
			// Clear this ability's slot, just in case, it's a different slot.
			ClearSlot(AbilitySpec);
			// Now, assign this ability to this slot
			AbilitySpec->DynamicAbilityTags.AddTag(Slot);
			if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
			{
				AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Unlocked);
				AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
			}
			MarkAbilitySpecDirty(*AbilitySpec);
		}
		ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PreviousSlot);
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot,
                                                     const FGameplayTag& PreviousSlot)
{
	AbilityEquippedDelegate.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

bool UAuraAbilitySystemComponent::GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
                                                             FString& OutNextLevelDescription)
{
	// 详见UAuraAbilitySystemComponent::GetSpecFromAbilityTag，由于GetActivatableAbilities
	// 所以对于未激活（Locked）的技能，无法根据AbilityTag获取到对应的AbilitySpec
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}
	// 未能根据AbilityTag找到Ability（处于锁定状态，显示锁定技能描述）
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None)) // 注意加上
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}
	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	Spec->DynamicAbilityTags.RemoveTag(Slot);
	MarkAbilitySpecDirty(*Spec);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&Spec, Slot))
		{
			ClearSlot(&Spec);
		}
	}
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	/*for (FGameplayTag Tag : Spec->DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}*/
	return Spec->DynamicAbilityTags.HasTagExact(Slot);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
                                                                           int32 AbilityLevel)
{
	AbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec,
                                                                     FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTagsDelegate.Broadcast(TagContainer);
}
