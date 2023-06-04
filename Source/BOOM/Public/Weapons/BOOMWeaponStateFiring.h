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
	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleFireInput() override;
};
