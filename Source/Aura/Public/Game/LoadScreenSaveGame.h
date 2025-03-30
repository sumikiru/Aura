// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

struct FGameplayTag;
class UGameplayAbility;

UENUM(BlueprintType)
enum class ESaveSlotStatus : uint8
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatus = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilitySlot = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityType = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel;
};

inline bool operator==(const FSavedAbility& Left, const FSavedAbility& Right)
{
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString SlotName = FString(); // 存储的插槽名称
	UPROPERTY()
	int32 SlotIndex = 0; // 存储的插槽索引
	// 以下属性需要在AAuraGameModeBase::SaveSlotData中更新
	UPROPERTY()
	FString PlayerName = FString("Default Name"); // 存档名称
	UPROPERTY()
	FString MapName = FString("Default Map Name"); // 地图名称
	UPROPERTY()
	FName PlayerStartTag;
	UPROPERTY()
	ESaveSlotStatus SaveSlotStatus = ESaveSlotStatus::Vacant;

	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	/** Player */
	UPROPERTY()
	int32 PlayerLevel = 1;
	UPROPERTY()
	int32 XP = 0;
	UPROPERTY()
	int32 SpellPoints = 0;
	UPROPERTY()
	int32 AttributePoints = 0;
	/** Primary Attributes */
	UPROPERTY()
	float Strength = 0.f;
	UPROPERTY()
	float Intelligence = 0.f;
	UPROPERTY()
	float Resilience = 0.f;
	UPROPERTY()
	float Vigor = 0.f;

	/** Abilities */
	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;
};
