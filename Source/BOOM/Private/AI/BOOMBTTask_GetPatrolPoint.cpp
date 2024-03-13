// Project BOOM


#include "AI/BOOMBTTask_GetPatrolPoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BOOMAICharacter.h"
#include "AI/BOOMPatrolRouteComponent.h"

UBOOMBTTask_GetPatrolPoint::UBOOMBTTask_GetPatrolPoint()
{
	NodeName = "GetPatrolPoint";
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBOOMBTTask_GetPatrolPoint, BlackboardKey));
}

EBTNodeResult::Type UBOOMBTTask_GetPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	const ABOOMCharacter* AICharacter = Cast<ABOOMCharacter>(AIController->GetCharacter());
	UBOOMPatrolRouteComponent* PatrolRoute = AIController->GetCharacter()->FindComponentByClass<UBOOMPatrolRouteComponent>();
	
	if (!AICharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character not assigned for: %s"),*GetNameSafe(AICharacter)) //getactor getnamesafe
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;

	}
	//UBOOMPatrolRouteComponent* PatrolRoute = AICharacter->GetPatrolRouteComponent();

	if (!PatrolRoute)
	{
		UE_LOG(LogTemp, Warning, TEXT("Patrol not assigned for: %s"), *GetNameSafe(AICharacter)) //getactor getnamesafe
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
