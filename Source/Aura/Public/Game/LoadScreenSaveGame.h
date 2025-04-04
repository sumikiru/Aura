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
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName = FName();
	UPROPERTY()
	FTransform Transform = FTransform(); // 位置、旋转等
	UPROPERTY()
	TArray<uint8> Bytes; // 来自Actor（仅对于那些标记为SaveGame的Actor）的序列化变量
};

// 重写==，用于AddUnique()
inline bool operator==(const FSavedActor& Left, const FSavedActor& Right)
{
	return Left.ActorName == Right.ActorName;
}

USTRUCT(BlueprintType)
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName = FString();
	UPROPERTY()
	TArray<FSavedActor> SavedActors;
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
	int32 AbilityLevel = 1;
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
	FString MapAssetName = FString("Default Map Asset Name");
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

	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	FSavedMap GetSavedMapWithMapName(const FString& InMapName) const;
	bool HasMap(const FString& InMapName) const;
};
