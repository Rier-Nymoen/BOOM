// Project BOOM


#include "AI/BOOMBTTask_FindNextPatrolPoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BOOMAICharacter.h"
#include "AI/BOOMPatrolRoute.h"

UBOOMBTTask_FindNextPatrolPoint::UBOOMBTTask_FindNextPatrolPoint()
{
	NodeName = "FindNextPatrolPoint";
}

EBTNodeResult::Type UBOOMBTTask_FindNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AAIController* AIController = OwnerComp.GetAIOwner();
	const ABOOMAICharacter* AICharacter = Cast<ABOOMAICharacter>(AIController->GetCharacter());

	if (!AICharacter)
	{
		//return fail
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Cyan, "Patrol has failed - AICharacter null");

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;

	}
	ABOOMPatrolRoute* PatrolRoute = AICharacter->GetPatrolRouteComponent();

	if (!PatrolRoute)
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Cyan, "Patrol has failed - PatrolRoute- null");

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}



	PatrolRoute->FindNextPoint();
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;

}

FString UBOOMBTTask_FindNextPatrolPoint::GetStaticDescription() const
{
	return FString::Printf(TEXT("FindsNextPatrolPointPath"));
}
