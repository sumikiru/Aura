// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UBTTask_Attack : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

	/*需要重写ExecuteTask方法，该方法在触发此任务(蓝图BTT_Attack中)时执行*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
