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
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, Weapon->ReloadDurationSeconds, FColor::Cyan, "Feed Load");
		Weapon->GetWorldTimerManager().SetTimer(TimerHandle_FeedReload, this, &UBOOMWeaponStateFeedReloading::CheckReloadTimer, Weapon->ReloadDurationSeconds, true);
		
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
