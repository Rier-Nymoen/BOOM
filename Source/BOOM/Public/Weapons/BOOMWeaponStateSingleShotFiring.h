// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BOOMWeaponStateFiring.h"
#include "BOOMWeaponStateSingleShotFiring.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMWeaponStateSingleShotFiring : public UBOOMWeaponStateFiring
{
	GENERATED_BODY()
	
public:
	UBOOMWeaponStateSingleShotFiring();
		
	virtual void BeginPlay() override;

	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleUnequipping() override;

	virtual void HandleReloadInput() override;
};
