// Project BOOM


#include "AI/BOOMBTTask_BurstGeometryFire.h"
#include "Character/BOOMCharacter.h"
#include "AIController.h"

UBOOMBTTask_BurstGeometryFire::UBOOMBTTask_BurstGeometryFire()
{
	NodeName = "TestBurstFireGeometry";
	bCreateNodeInstance = 1;
	bTickIntervals = true;
	INIT_TASK_NODE_NOTIFY_FLAGS();

}

EBTNodeResult::Type UBOOMBTTask_BurstGeometryFire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//UE_LOG(LogTemp, Warning, TEXT("UBOOMBTTask_BurstGeometryFire::InProgress()"))
	AAIController* AIController = OwnerComp.GetAIOwner();
	ABOOMCharacter* AICharacter = Cast<ABOOMCharacter>(AIController->GetCharacter());
	if (AICharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateNodeInstance: %d"), bCreateNodeInstance)

		float Duration = AICharacter->CombatProperties.BurstDurationSeconds;
		bNotifyTick = true;
		SetNextTickTime(NodeMemory, Duration);
		TimerDelegate_RateOfFire = FTimerDelegate::CreateUObject(this, &UBOOMBTTask_BurstGeometryFire::Fire, AICharacter );
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_RateOfFire, TimerDelegate_RateOfFire, AICharacter->CombatProperties.RateOfFiringInputSeconds, true);
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;

}

FString UBOOMBTTask_BurstGeometryFire::GetStaticDescription() const
{
	return FString("TestingBurstFireGeometry");
}

void UBOOMBTTask_BurstGeometryFire::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTTaskMemory* TaskMemory = GetSpecialNodeMemory<FBTTaskMemory>(NodeMemory);
	if (TaskMemory)
	{
		if (TaskMemory->NextTickRemainingTime <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("BurstGeometrySuccess"))

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

void UBOOMBTTask_BurstGeometryFire::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RateOfFire);
}

/*Read up on how the node memory works.*/
void UBOOMBTTask_BurstGeometryFire::Fire(ABOOMCharacter* AICharacter)
{
	if (AICharacter)
	{
		AICharacter->SingleFire();
	}
}

