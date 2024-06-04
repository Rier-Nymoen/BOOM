// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BOOMAIController.generated.h"


/**
 * 
 */
UCLASS()
class BOOM_API ABOOMAIController : public AAIController
{
	GENERATED_BODY()
public:

	ABOOMAIController();

protected:

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UAISenseConfig_Sight* AISenseConfig_Sight;

	UFUNCTION()
	virtual void OnTargetPerceptionForgotten(AActor* ForgottenActor);

	//UFUNCTION()
	//virtual void OnTargetPerceptionUpdated(FActorPerceptionUpdateInfo& UpdateInfo);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName InCombatKeyName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName TargetActorKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName LastKnownPositionKeyName;

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
