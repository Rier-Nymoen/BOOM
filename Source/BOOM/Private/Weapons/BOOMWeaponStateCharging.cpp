// Project BOOM


#include "Weapons/BOOMWeaponStateCharging.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "Weapons/BOOMWeapon.h"

UBOOMWeaponStateCharging::UBOOMWeaponStateCharging()
{
	//ChargeRate = 50.0F;
	//CurrentCharge = 0.0F;
	//ChargedShotCost = 10.0F;
	//UnchargedShotCost = 3.0F;
	//ChargeThreshold = 100.0F;
	//ChargeIncrementTimeSeconds = 0.91F;
}

void UBOOMWeaponStateCharging::BeginPlay()
{

}

void UBOOMWeaponStateCharging::EnterState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{


	}

}



void UBOOMWeaponStateCharging::ExitState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{


	}
}

