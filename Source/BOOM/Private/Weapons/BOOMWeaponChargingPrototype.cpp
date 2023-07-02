// Project BOOM


#include "Weapons/BOOMWeaponChargingPrototype.h"
#include "Weapons/BOOMWeaponStateCharging.h"
ABOOMWeaponChargingPrototype::ABOOMWeaponChargingPrototype()
{
	//@TODO fill out weapon stats
	FiringState = CreateDefaultSubobject<UBOOMWeaponStateCharging>("ChargingState");
}

void ABOOMWeaponChargingPrototype::BeginPlay()
{
	Super::BeginPlay();

}
