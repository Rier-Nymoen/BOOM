// Project BOOM


#include "Weapons/BOOMWeaponDMR.h"
#include "Weapons/BOOMWeaponStateBurstFiring.h"

ABOOMWeaponDMR::ABOOMWeaponDMR()
{


	MagazineSize = 33.0F;
	MaxAmmoReserves = 210;
	CurrentAmmoReserves = 114;
	CurrentAmmo = MagazineSize;
	FiringState = CreateDefaultSubobject<UBOOMWeaponStateBurstFiring>("BurstFiringState");
}
