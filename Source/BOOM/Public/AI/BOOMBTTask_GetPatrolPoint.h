// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BOOMBTTask_GetPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMBTTask_GetPatrolPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBOOMBTTask_GetPatrolPoint();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;
};
