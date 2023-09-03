// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BOOMWeaponState.h"
#include "BOOMWeaponStateFiring.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMWeaponStateFiring : public UBOOMWeaponState
{
	GENERATED_BODY()

public:

	UBOOMWeaponStateFiring();


	//Interface implementations
	virtual void BeginPlay() override;

	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleUnequipping() override;

	virtual void HandleReloadInput() override;

	//End of interface implementations

	//Rate of Fire and Time before fire input can be processed after firing the weapon.
	float FireRateSeconds;

protected:
	virtual void CheckRefireTimer();

	FTimerHandle TimerHandle_RefireTimer;
};
