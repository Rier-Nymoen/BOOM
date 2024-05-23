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
		bool bIsClockwise = FMath::RandBool();

		if (bIsClockwise)
		{
			DirectionModifier = 1;
		}
		else
		{
			DirectionModifier = -1;
		}
		TaskDurationSeconds = 0;
		AICharacter->BurstGeometryProperties.CurrentBurstAngle = AICharacter->BurstGeometryProperties.InitialBurstAngle * DirectionModifier;

		float Duration = AICharacter->BurstGeometryProperties.BurstDurationSeconds;
		bNotifyTick = true;
		TimerDelegate_RateOfFire = FTimerDelegate::CreateUObject(this, &UBOOMBTTask_BurstGeometryFire::Fire, AICharacter);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_RateOfFire, TimerDelegate_RateOfFire, AICharacter->BurstGeometryProperties.RateOfFiringInputSeconds, true);

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;

}

FString UBOOMBTTask_BurstGeometryFire::GetStaticDescription() const
{
	return FString("BurstGeometryFire");
}

void UBOOMBTTask_BurstGeometryFire::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	ABOOMCharacter* AICharacter = Cast<ABOOMCharacter>(AIController->GetCharacter());
	if (AICharacter)
	{
		AICharacter->BurstGeometryProperties.CurrentBurstAngle -= AICharacter->BurstGeometryProperties.BurstCorrectionAngularVelocity * DeltaSeconds * DirectionModifier;
		if (DirectionModifier == 1)
		{
			AICharacter->BurstGeometryProperties.CurrentBurstAngle = FMath::Clamp(AICharacter->BurstGeometryProperties.CurrentBurstAngle, 0.f, 100.f);
		}
		else
		{
			AICharacter->BurstGeometryProperties.CurrentBurstAngle = FMath::Clamp(AICharacter->BurstGeometryProperties.CurrentBurstAngle, -100.f, 0.f);
		}
	}

	FBTBurstGeometryFireTaskMemory* TaskMemory = GetSpecialNodeMemory<FBTBurstGeometryFireTaskMemory>(NodeMemory);
	if (TaskMemory)
	{
		TaskDurationSeconds += DeltaSeconds;
		if (TaskDurationSeconds >= AICharacter->BurstGeometryProperties.BurstDurationSeconds)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

void UBOOMBTTask_BurstGeometryFire::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RateOfFire);
}

uint16 UBOOMBTTask_BurstGeometryFire::GetInstanceMemorySize() const
{
	return sizeof(FBTBurstGeometryFireTaskMemory);
}

void UBOOMBTTask_BurstGeometryFire::Fire(ABOOMCharacter* AICharacter)
{
	if (AICharacter)
	{
		AICharacter->SingleFire();
	}
}

