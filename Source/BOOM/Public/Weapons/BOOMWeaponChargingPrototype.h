// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BOOMWeapon.h"
#include "BOOMWeaponChargingPrototype.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API ABOOMWeaponChargingPrototype : public ABOOMWeapon
{
	GENERATED_BODY()
	
public:

	ABOOMWeaponChargingPrototype();

	class UBOOMWeaponState* ChargingState;


	//@TODO consider using timelines instead

protected:
	virtual void BeginPlay() override;


	FTimerDynamicDelegate TimerEventTest;

};
