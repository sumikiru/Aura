// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = AIOwner->GetPawn();
	// 记得先在角色蓝图中添加Tag
	// 如果是Player，目标为Enemy;如果是Enemy, 目标是Player
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");
	TArray<AActor*> ActorsWithTag;
	// 开销较大，不适合Tick中使用。不过这是在C++中编写并且已经在蓝图中设置每0.5s Tick一次，所有开销相对小了一点
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);

	float ClosestDistance = TNumericLimits<float>::Max();// 设置为float最大值，类比INT_MAX
	AActor* ClosestActor = nullptr;
	for (AActor* Actor : ActorsWithTag)
	{
		if (IsValid(Actor) && IsValid(OwningPawn))
		{
			const float Distance = OwningPawn->GetDistanceTo(Actor);
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = Actor;
			}
		}
	}
	/**
	 * 在蓝图中可以看到，行为树中BTService_FindNearestPlayer中的TargetToFollowSelector选择了TargetToFollow
	 * 但是这个cpp文件和蓝图中并没有”直接“设置TargetToFollow的值，那到底是怎么修改的呢？
	 * 查看UBTFunctionLibrary::SetBlackboardValueAsObject源码，发现BlackboardComp->SetValue<UBlackboardKeyType_Object>(Key.SelectedKeyName, Value);
	 * 可以知道TargetToFollowSelector.SelectedKeyName就是FName("TargetToFollow")，SetBlackboardValueAsObject本质上是根据TargetToFollowSelector来设置其对应的BlackboardKey的值（即TargetToFollow）
	 * GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Orange, TargetToFollowSelector.SelectedKeyName.ToString());
	 */	 
	// 设置BlackBoardKey
	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);
	
}
