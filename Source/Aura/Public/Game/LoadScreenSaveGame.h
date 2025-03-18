// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

UENUM(BlueprintType)
enum class ESaveSlotStatus : uint8
{
	Vacant,
	EnterName,
	Taken
};

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
};
