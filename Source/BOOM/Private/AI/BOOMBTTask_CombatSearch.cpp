// Project BOOM


#include "AI/BOOMBTTask_CombatSearch.h"
#include "Character/BOOMCharacter.h"
#include "AI/BOOMAIController.h"
UBOOMBTTask_CombatSearch::UBOOMBTTask_CombatSearch()
{
	NodeName = "Do Combat Search";
;}

EBTNodeResult::Type UBOOMBTTask_CombatSearch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABOOMAIController* BOOMAIController = Cast<ABOOMAIController>(OwnerComp.GetAIOwner());

	ABOOMCharacter* BOOMCharacter = Cast<ABOOMCharacter>(BOOMAIController->GetPawn());
	
	if (BOOMCharacter)
	{

	}


	return EBTNodeResult::Type();
}

FString UBOOMBTTask_CombatSearch::GetStaticDescription() const
{
	return FString();
}
