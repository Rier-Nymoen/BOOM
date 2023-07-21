// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BOOMBTTask_Patrol.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMBTTask_Patrol : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBOOMBTTask_Patrol();
protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;
};
