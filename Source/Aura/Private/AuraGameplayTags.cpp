// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

/**
 * 需要在.cpp中显式声明静态变量类型
 */
FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/**
	 * Primary Attributes
	 */
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Primary.Strength")),
		FString(TEXT("Increases physical damage"))
	);

	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Primary.Intelligence")),
		FString(TEXT("Increases magical damage"))
	);

	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Primary.Resilience")),
		FString(TEXT("Increases Armor and Armor Penetration"))
	);

	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Primary.Vigor")),
		FString(TEXT("Increases Health"))
	);

	/**
	 * Seconary Attributes
	 */
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.Armor")),
		FString(TEXT("Reduces damage taken, improve Block Chance"))
	);

	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.ArmorPenetration")),
		FString(TEXT("Ignored Percentage of enemy Armor, increases Critical Hit Chance"))
	);

	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.BlockChance")),
		FString(TEXT("Chance to cut incoming damage in half"))
	);

	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.CriticalHitChance")),
		FString(TEXT("Chance to double damage plus critical hit bonus"))
	);

	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.CriticalHitDamage")),
		FString(TEXT("Bonus damage added when a critical hit is scored"))
	);

	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.CriticalHitResistance")),
		FString(TEXT("Reduces Critical Hit Chance of attacking enemies"))
	);

	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.HealthRegeneration")),
		FString(TEXT("Amount of Health regenerated per second"))
	);

	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.ManaRegeneration")),
		FString(TEXT("Amount of Mana regenerated per second"))
	);

	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.MaxHealth")),
		FString(TEXT("Maximum amount of Health obtainable"))
	);

	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Secondary.MaxMana")),
		FString(TEXT("Maximum amount of Mana obtainable"))
	);

	/**
	 * Input Tags
	 */
	GameplayTags.InputTag_LeftMouseButton = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.LeftMouseButton")),
		FString(TEXT("Input Tag for Left Mouse Button"))
	);

	GameplayTags.InputTag_RightMouseButton = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.RightMouseButton")),
		FString(TEXT("Input Tag for Right Mouse Button"))
	);

	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.1")),
		FString(TEXT("Input Tag for 1 key"))
	);

	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.2")),
		FString(TEXT("Input Tag for 2 key"))
	);

	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.3")),
		FString(TEXT("Input Tag for 3 key"))
	);

	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("InputTag.4")),
		FString(TEXT("Input Tag for 4 key"))
	);

	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage")),
		FString(TEXT("Damage"))
	);

	/**
	 * Damage Types
	 */
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage.Fire")),
		FString(TEXT("Fire Damage Type"))
	);

	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage.Lightning")),
		FString(TEXT("Lightning Damage Type"))
	);

	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage.Arcane")),
		FString(TEXT("Arcane Damage Type"))
	);

	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Damage.Physical")),
		FString(TEXT("Physical Damage Type"))
	);

	/**
	 * Resistances
	 */
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Resistance.Fire")),
		FString(TEXT("Resistance to Fire Damage"))
	);
	
	GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Resistance.Arcane")),
		FString(TEXT("Resistance to Arcane Damage"))
	);
	
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Resistance.Lightning")),
		FString(TEXT("Resistance to Lightning Damage"))
	);

	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Attributes.Resistance.Physical")),
		FString(TEXT("Resistance to Physical Damage"))
	);

	/**
	 * Map of Damage Types to Resistances
	 */
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);

	/**
	 * Effects
	 */
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Effects.HitReact")),
		FString(TEXT("Tag granted when Hit Reacting"))
	);

	/**
	 * Abilities
	 */
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Attack")),
		FString(TEXT("Attack Ability Tag"))
	);
	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Abilities.Summon")),
		FString(TEXT("Summon Ability Tag"))
	);

	/**
	 * Combat Sockets
	 */
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("CombatSocket.Weapon")),
		FString(TEXT("Weapon"))
	);
	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("CombatSocket.LeftHand")),
		FString(TEXT("Left Hand"))
	);
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("CombatSocket.RightHand")),
		FString(TEXT("Right Hand"))
	);
	GameplayTags.CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("CombatSocket.Tail")),
		FString(TEXT("Tail"))
	);

	/**
	 * Montages
	 */
	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Montage.Attack.1")),
		FString(TEXT("Attack 1"))
	);
	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Montage.Attack.2")),
		FString(TEXT("Attack 2"))
	);
	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Montage.Attack.3")),
		FString(TEXT("Attack 3"))
	);
	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TEXT("Montage.Attack.4")),
		FString(TEXT("Attack 4"))
	);
}
