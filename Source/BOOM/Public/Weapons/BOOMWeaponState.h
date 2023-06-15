// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BOOMStateBase.h"
#include "BOOMWeaponState.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMWeaponState : public UBOOMStateBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	
	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleFireInput();
	
	virtual void HandleReloadInput();

	virtual void HandleEquipping();

	virtual void HandleUnequipping();

	virtual void HandleStopFiringInput();
	


protected:
	virtual void InitializeStateComponent();

	//@TODO debating using this.
	class ABOOMWeapon* OwningWeapon;
	
};
