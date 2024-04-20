// Project BOOM


#include "AI/BOOMAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"


ABOOMAIController::ABOOMAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComponent");
}


void ABOOMAIController::BeginPlay()
{
	Super::BeginPlay();
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ABOOMAIController::OnTargetPerceptionForgotten);

	if (IsValid(BehaviorTree.Get()))
	{
		RunBehaviorTree(BehaviorTree.Get());
		BehaviorTreeComponent->StartTree(*BehaviorTree.Get());
	}
	
}

void ABOOMAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	BlackboardComponent->SetValueAsBool(FName(TEXT("InCombat")), true);

	if (IsValid(Blackboard.Get()) && IsValid(BehaviorTree.Get()))
	{
		Blackboard->InitializeBlackboard(*BehaviorTree.Get()->BlackboardAsset.Get());
	}
}

void ABOOMAIController::OnTargetPerceptionForgotten(AActor* ForgottenActor)
{
	TArray<AActor*> PerceivedHostileActors;
	AIPerceptionComponent->GetPerceivedHostileActors(PerceivedHostileActors);
	if (PerceivedHostileActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No hostile actors are perceived - marks end of combat for all AI controllers."))
		if (IsValid(Blackboard.Get()))
		{
			BlackboardComponent->SetValueAsBool(FName(TEXT("InCombat")), false);
			//First we'll see if EQS Get all perceived actors returns unexpired actor information.
			//Then we need to make tasks to update the blackboard states.
		}
	}
}

void ABOOMAIController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	Super::SetGenericTeamId(NewTeamID);
}