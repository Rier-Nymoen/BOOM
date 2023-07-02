// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BOOMWeaponStateFiring.h"
#include "BOOMWeaponStateCharging.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMWeaponStateCharging : public UBOOMWeaponStateFiring
{
	GENERATED_BODY()


public:


	UBOOMWeaponStateCharging();


	virtual void BeginPlay() override;

	virtual void EnterState() override;

	virtual void ExitState() override;


	virtual void HandleStopFiringInput() override;

	//Increment amount of weapon per increment time
	float ChargeRate;

	UFUNCTION()
	virtual void AddCharge(float Amount);

protected:
	//@TODO consider making charged weapons with other reload types. need more components for that.


	
	float CurrentCharge;

	//Level of charge needed to produce charged shot
	float ChargeThreshold;

	//How often we add ChargeRate.
	float ChargeIncrementTimeSeconds;

	float ChargedShotCost;

	float UnchargedShotCost;

	FTimerHandle TimerHandle_IncrementCharge;

	FTimerDelegate ChargeDelegate;

};
