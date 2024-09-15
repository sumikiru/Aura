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
}
