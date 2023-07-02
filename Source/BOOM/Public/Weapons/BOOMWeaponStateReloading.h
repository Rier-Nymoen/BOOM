// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BOOMWeaponState.h"
#include "BOOMWeaponStateReloading.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMWeaponStateReloading : public UBOOMWeaponState
{
	GENERATED_BODY()
	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleFireInput() override;

	virtual void HandleEquipping() override;

	virtual void HandleUnequipping() override;

public:

	FTimerHandle TimerHandle_ReloadWeapon;

private:
	void CancelReload();

};
