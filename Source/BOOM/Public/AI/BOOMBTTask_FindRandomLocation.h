// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BOOMBTTask_FindRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMBTTask_FindRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	UBOOMBTTask_FindRandomLocation();
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	float SearchRadius = 1000.0F;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

};
