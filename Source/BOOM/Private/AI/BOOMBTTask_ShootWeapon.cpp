// Project BOOM


#include "AI/BOOMBTTask_ShootWeapon.h"
#include "AIController.h"
#include "BOOMAICharacter.h"

UBOOMBTTask_ShootWeapon::UBOOMBTTask_ShootWeapon()
{
}

EBTNodeResult::Type UBOOMBTTask_ShootWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AAIController* AIController = OwnerComp.GetAIOwner();
	const ABOOMAICharacter* AICharacter = Cast<ABOOMAICharacter>(AIController->GetCharacter());


	if (AICharacter)
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Emerald, "SUCCESSFUL AI CHARACTER READ");
	
	}
	else
	{		
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Red, "Unsuccessful");
	}

	return EBTNodeResult::Type();
}

FString UBOOMBTTask_ShootWeapon::GetStaticDescription() const
{
	return FString();
}
