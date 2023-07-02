// Project BOOM


#include "Weapons/BOOMWeaponStateBurstFiring.h"
#include "Weapons/BOOMWeapon.h"

UBOOMWeaponStateBurstFiring::UBOOMWeaponStateBurstFiring()
{
	BurstSize = 3;
	BurstInterval = 0.15F;
}

void UBOOMWeaponStateBurstFiring::BeginPlay()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon != nullptr)
	{
		BurstInterval = Weapon->GetFireRateSeconds()/BurstSize;

	}
}

void UBOOMWeaponStateBurstFiring::EnterState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon != nullptr)
	{
		Weapon->Fire();
		CurrentShot++;
		Weapon->GetWorldTimerManager().SetTimer(TimerHandle_RefireTimer, this, &UBOOMWeaponStateBurstFiring::CheckRefireTimer, BurstInterval, true);
	}

}

void UBOOMWeaponStateBurstFiring::ExitState()
{
	//Super::ExitState();
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor(120, 0, 200), "Ended burst state");
		Weapon->GetWorldTimerManager().ClearTimer(TimerHandle_RefireTimer);
		CurrentShot = 0;
	}
}

void UBOOMWeaponStateBurstFiring::HandleUnequipping()
{
	Super::HandleUnequipping();
}

void UBOOMWeaponStateBurstFiring::CheckRefireTimer()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon != nullptr)
	{
			if (CurrentShot == BurstSize || Weapon->CurrentAmmo == 0)
			{
				Weapon->GotoState(Weapon->ActiveState);
				return;
			}
			Weapon->Fire();
			CurrentShot++;
	}
}
