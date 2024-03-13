// Project BOOM


#include "AI/BOOMBTTask_FindNextPatrolPoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BOOMAICharacter.h"
#include "AI/BOOMPatrolRouteComponent.h"

UBOOMBTTask_FindNextPatrolPoint::UBOOMBTTask_FindNextPatrolPoint()
{
	NodeName = "FindNextPatrolPoint";
}

EBTNodeResult::Type UBOOMBTTask_FindNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AAIController* AIController = OwnerComp.GetAIOwner();
	const ABOOMCharacter* AICharacter = Cast<ABOOMCharacter>(AIController->GetCharacter());

	if (!AICharacter)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	//UBOOMPatrolRouteComponent* PatrolRoute = AICharacter->GetPatrolRouteComponent();
	UBOOMPatrolRouteComponent* PatrolRoute = AIController->GetCharacter()->FindComponentByClass<UBOOMPatrolRouteComponent>();
	if (!PatrolRoute)
	{
		UE_LOG(LogTemp, Warning, TEXT("Patrol not assigned for: %s"), *GetNameSafe(AICharacter)) //getactor getnamesafe

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
