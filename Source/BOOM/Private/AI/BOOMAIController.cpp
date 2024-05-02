// Project BOOM


#include "AI/BOOMAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

#include "Perception/AISense_Damage.h"


ABOOMAIController::ABOOMAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComponent");
	InCombatName = FName(TEXT("InCombat"));
}


void ABOOMAIController::BeginPlay()
{
	Super::BeginPlay();
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ABOOMAIController::OnTargetPerceptionForgotten);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABOOMAIController::OnTargetPerceptionUpdated);
	if (IsValid(BehaviorTree.Get()))
	{
		RunBehaviorTree(BehaviorTree.Get());
		BehaviorTreeComponent->StartTree(*BehaviorTree.Get());
	}
	
}

void ABOOMAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

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
			BlackboardComponent->SetValueAsBool(InCombatName, false);
		}
	}
}

void ABOOMAIController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	Super::SetGenericTeamId(NewTeamID);
}

void ABOOMAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	
}



