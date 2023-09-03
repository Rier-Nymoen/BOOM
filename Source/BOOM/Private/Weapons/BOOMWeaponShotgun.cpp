// Project BOOM


#include "Weapons/BOOMWeaponShotgun.h"

#include "Weapons/BOOMWeaponStateFeedReloading.h"


// Sets default values
ABOOMWeaponShotgun::ABOOMWeaponShotgun()
{
	ReloadingState = CreateDefaultSubobject<UBOOMWeaponStateFeedReloading>("FeedReloadingState");
}

