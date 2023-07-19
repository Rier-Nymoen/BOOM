// Project BOOM


#include "Weapons/BOOMWeaponChargingPrototype.h"
#include "Weapons/BOOMWeaponStateCharging.h"
ABOOMWeaponChargingPrototype::ABOOMWeaponChargingPrototype()
{
	FiringState = CreateDefaultSubobject<UBOOMWeaponStateCharging>("ChargingState");
}

void ABOOMWeaponChargingPrototype::BeginPlay()
{
	Super::BeginPlay();

}
