// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ULootTiers;
class ULoadScreenSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	UPROPERTY(EditDefaultsOnly, Category = "Loot Tiers Info")
	TObjectPtr<ULootTiers> LootTiersInfo;

	// 存档：要存储的存档插槽，以及其对应的索引
	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);
	// 获取存档插槽记录的存档信息
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	// 删除存档
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);
	// 检索游戏内存档信息
	ULoadScreenSaveGame* RetrieveInGameSaveData();
	void SaveInGameProgressData(ULoadScreenSaveGame* SaveObject);
	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	void LoadWorldState(UWorld* World) const;
	void TravelToMap(UMVVM_LoadSlot* LoadSlot);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	// 游戏初始地图
	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;
	// 使用了TSoftObjectPtr，这个指针只保存路径，如果不使用，对应的资源不会加载到场景，可以在需要时再加载。常用于延迟加载的资源（如关卡、角色、贴图等）
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;
	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;

	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void PlayerDied(ACharacter* DeadCharacter);

protected:
	virtual void BeginPlay() override;
};
