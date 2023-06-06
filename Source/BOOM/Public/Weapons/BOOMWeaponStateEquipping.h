// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BOOMWeaponState.h"
#include "BOOMWeaponStateEquipping.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMWeaponStateEquipping : public UBOOMWeaponState
{
	GENERATED_BODY()
public:

	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleFireInput() override;

	virtual void HandleReloadInput() override;

	virtual void HandleEquipping() override;
};
