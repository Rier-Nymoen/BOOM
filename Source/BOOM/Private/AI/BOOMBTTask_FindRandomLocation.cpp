// Project BOOM


#include "AI/BOOMBTTask_FindRandomLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBOOMBTTask_FindRandomLocation::UBOOMBTTask_FindRandomLocation()
{
    NodeName = "FindRandomLocation";
    BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBOOMBTTask_FindRandomLocation, BlackboardKey));
}

EBTNodeResult::Type UBOOMBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    FNavLocation Destination;

    AAIController* AIController = OwnerComp.GetAIOwner();
    const APawn* AIPawn = AIController->GetPawn();

    const FVector Origin = AIPawn->GetActorLocation();

    const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Emerald, "task executed");
    if (IsValid(NavSystem) && NavSystem->GetRandomPointInNavigableRadius(Origin, SearchRadius, Destination))
    {
        AIController->GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, Destination.Location);
    }
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}

FString UBOOMBTTask_FindRandomLocation::GetStaticDescription() const
{
    return FString::Printf(TEXT("Testing"));
}
