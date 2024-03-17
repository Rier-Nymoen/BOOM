// Project BOOM


#include "AI/BOOMBTTask_CombatSearch.h"
#include "AI/BOOMAIController.h"
UBOOMBTTask_CombatSearch::UBOOMBTTask_CombatSearch()
{
	NodeName = "Do Combat Search";
;}

EBTNodeResult::Type UBOOMBTTask_CombatSearch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABOOMAIController* BOOMAIController = Cast<ABOOMAIController>(OwnerComp.GetAIOwner());



	/*
	Owner must have perceived something and should focus on what it heard.

	So now we need to search and if we get a certain observation like sight on an enemy fire an event to do combat tactics



	*/

	return EBTNodeResult::Type();
}

FString UBOOMBTTask_CombatSearch::GetStaticDescription() const
{
	return FString();
}
