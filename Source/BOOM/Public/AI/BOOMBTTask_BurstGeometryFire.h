// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BOOMBTTask_BurstGeometryFire.generated.h"

/**
 * 
 */


UCLASS()
class BOOM_API UBOOMBTTask_BurstGeometryFire : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:

	UBOOMBTTask_BurstGeometryFire();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);


	//if each unit has their own instance of this... it should be fine.
	FTimerHandle TimerHandle_RateOfFire;

	FTimerDelegate TimerDelegate_RateOfFire;
	
	void Fire(class ABOOMCharacter* AICharacter);
};
