// Project BOOM


#include "Weapons/BOOMWeaponStateSingleShotFiring.h"
#include "Weapons/BOOMWeapon.h"

UBOOMWeaponStateSingleShotFiring::UBOOMWeaponStateSingleShotFiring()
{
}

void UBOOMWeaponStateSingleShotFiring::BeginPlay()
{
	Super::BeginPlay();
}

void UBOOMWeaponStateSingleShotFiring::EnterState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon != nullptr)
	{
		Weapon->Fire();
		Weapon->GotoStateActive();
	}
}

void UBOOMWeaponStateSingleShotFiring::ExitState()
{
}

void UBOOMWeaponStateSingleShotFiring::HandleUnequipping()
{
}

void UBOOMWeaponStateSingleShotFiring::HandleReloadInput()
{
}
