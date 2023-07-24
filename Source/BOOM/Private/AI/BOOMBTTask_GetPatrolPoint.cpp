// Project BOOM


#include "AI/BOOMBTTask_GetPatrolPoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BOOMAICharacter.h"
#include "AI/BOOMPatrolRoute.h"

UBOOMBTTask_GetPatrolPoint::UBOOMBTTask_GetPatrolPoint()
{
	NodeName = "GetPatrolPoint";
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBOOMBTTask_GetPatrolPoint, BlackboardKey));
}

EBTNodeResult::Type UBOOMBTTask_GetPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	const ABOOMAICharacter* AICharacter = Cast<ABOOMAICharacter>(AIController->GetCharacter());

	if (!AICharacter)
	{
		//return fail
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Cyan, "Patrol has failed - AICharacter null");

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;

	}
	ABOOMPatrolRoute* PatrolRoute = AICharacter->GetPatrolRouteComponent();

	if (!PatrolRoute)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Cyan, "Patrol has failed - PatrolRoute- null");

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	//WORLD SPACE
	AIController->GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, PatrolRoute->GetCurrentPoint());

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBOOMBTTask_GetPatrolPoint::GetStaticDescription() const
{
	return FString::Printf(TEXT("Gets current patrol point"));
}
