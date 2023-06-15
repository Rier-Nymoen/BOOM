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

	virtual void BeginPlay() override;


	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleFireInput() override;

	virtual void HandleUnequipping() override;

	virtual void HandleStopFiringInput() override;


protected:
	//Time before fire input can be processed after firing the weapon.
	float FireCooldownSeconds; 
	bool bIsOnFireCooldown;
	virtual void EndFireCooldown();

	FTimerHandle TimerHandle_FireCooldown;
};
