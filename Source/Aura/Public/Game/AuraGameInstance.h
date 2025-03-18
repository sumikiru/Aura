// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 * 保存特定类型的持久数据，如用户账户信息
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName PlayerStartTag = FName();
	UPROPERTY()
	FString LoadSlotName = FString(); //当前使用的或后续保存内容到的存档名称
	UPROPERTY()
	int32 LoadSlotIndex = 0;
};
