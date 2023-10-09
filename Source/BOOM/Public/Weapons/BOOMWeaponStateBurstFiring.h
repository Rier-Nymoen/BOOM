// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BOOMWeaponStateFiring.h"
#include "BOOMWeaponStateBurstFiring.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMWeaponStateBurstFiring : public UBOOMWeaponStateFiring
{
	GENERATED_BODY()
	

public:

	UBOOMWeaponStateBurstFiring();


	//Interface implementations
	virtual void BeginPlay() override;

	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleUnequipping() override;

	//End of interface implementations

protected:
	virtual void CheckRefireTimer() override;

	//related to fire rate seconds
	int BurstSize;

	int CurrentShot;

	//must be function of firerate or something
	float BurstInterval;


};
/*
	
	CurrentState->





*/