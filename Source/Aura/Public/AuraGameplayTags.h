// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 删除类声明和构造函数、析构函数，而是声明一个结构体。\n
 * AuraGameplayTags :\n
 * Singleton containing native Gameplay tags(Singleton Pattern : 单例模式)
 */

struct FAuraGameplayTags
{
public:
    static const FAuraGameplayTags& Get() { return GameplayTags; }
    static void InitializeNativeGameplayTags();

    /** 使用下划线代替点 */
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;
	
    FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;

	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;
	
	/** Input Tags */
	FGameplayTag InputTag_LeftMouseButton;
	FGameplayTag InputTag_RightMouseButton;
	//FGameplayTag InputTag_MiddleMouseButton;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	FGameplayTag Abilities_Attack;

	FGameplayTag Montage_Attack_Weapon;
	FGameplayTag Montage_Attack_LeftHand;
	FGameplayTag Montage_Attack_RightHand;

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
 	
	FGameplayTag Effects_HitReact;

private:
    static FAuraGameplayTags GameplayTags;
};
