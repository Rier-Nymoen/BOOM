// Project BOOM


#include "AI/BOOMBTTask_ShootWeapon.h"
#include "AIController.h"
#include "BOOMAICharacter.h"

UBOOMBTTask_ShootWeapon::UBOOMBTTask_ShootWeapon()
{
	NodeName = "ShootWeapon";
}

EBTNodeResult::Type UBOOMBTTask_ShootWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AAIController* AIController = OwnerComp.GetAIOwner();
	ABOOMAICharacter* AICharacter = Cast<ABOOMAICharacter>(AIController->GetCharacter());


	if (AICharacter)
	{
		const UWorld* World = AICharacter->GetWorld();
		if (World)
		{
		}

		AICharacter->StartFire();
		AICharacter->StopFire();
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBOOMBTTask_ShootWeapon::GetStaticDescription() const
{
	return FString("Fires weapon lol");
}
