// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BOOMBTTask_CombatSearch.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMBTTask_CombatSearch : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UBOOMBTTask_CombatSearch();

protected:


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;
};
