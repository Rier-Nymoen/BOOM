// Project BOOM


#include "AI/BOOMBTTask_Patrol.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBOOMBTTask_Patrol::UBOOMBTTask_Patrol()
{
	NodeName = "FindPatrolPoint";

	/*
	Have patrol path component to share behaviors among groups of classes.
	*/
}

EBTNodeResult::Type UBOOMBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Move to spline form.
	AAIController* AIController = OwnerComp.GetAIOwner();
	const APawn* AIPawn = AIController->GetPawn();
	

	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBOOMBTTask_Patrol::GetStaticDescription() const
{
	return FString::Printf(TEXT("Picks a patrol point to move towards"));
}
