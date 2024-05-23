// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BOOMBTTask_BurstGeometryFire.generated.h"

/**
 * 
 */
struct FBTBurstGeometryFireTaskMemory : public FBTTaskMemory
{
public:

	float TaskDurationSeconds;
};


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

	uint16 GetInstanceMemorySize() const override;

	//if each unit has their own instance of this... it should be fine.
	FTimerHandle TimerHandle_RateOfFire;

	FTimerDelegate TimerDelegate_RateOfFire;

	float TaskDurationSeconds;

	int DirectionModifier;

	void Fire(class ABOOMCharacter* AICharacter);
};
