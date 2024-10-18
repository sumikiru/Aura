// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AI/AuraAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAuraAIController::AAuraAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	checkf(Blackboard, TEXT("BlackboardComponent is not found!"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	checkf(BehaviorTreeComponent, TEXT("BehaviorTreeComponent is not found!"));
}
