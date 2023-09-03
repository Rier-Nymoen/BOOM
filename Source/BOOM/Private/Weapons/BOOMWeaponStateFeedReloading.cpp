// Project BOOM


#include "Weapons/BOOMWeaponStateFeedReloading.h"
#include "Weapons/BOOMWeapon.h"

void UBOOMWeaponStateFeedReloading::EnterState()
{

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if(Weapon)
	{
		if(!Weapon->CanReload())
		{
			Weapon->GotoStateActive();
			return;	
		}

		Weapon->GetWorldTimerManager().SetTimer(TimerHandle_FeedReload, this, &UBOOMWeaponStateFeedReloading::CheckReloadTimer, 0.7F, true);
		
	}
	
}

void UBOOMWeaponStateFeedReloading::ExitState()
{

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if(Weapon)
	{
		Weapon->GetWorldTimerManager().ClearTimer(TimerHandle_FeedReload);
	}
	
}

void UBOOMWeaponStateFeedReloading::HandleFireInput()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if(Weapon)
	{
		Weapon->GotoState(Weapon->FiringState);
	}
}

void UBOOMWeaponStateFeedReloading::CheckReloadTimer()
{
	
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if(Weapon)
	{
		if(!Weapon->CanReload())
		{
			Weapon->GotoStateActive();
			return;
		}

		Weapon->FeedReloadWeapon();
	}

}
