// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BOOMWeaponStateActive.h"
#include "Weapons/BOOMWeaponStateFiring.h"

#include "Weapons/BOOMWeapon.h"
void UBOOMWeaponStateActive::EnterState()
{

}

void UBOOMWeaponStateActive::ExitState()
{
}

void UBOOMWeaponStateActive::HandleFireInput()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->FiringState);
	}
}

void UBOOMWeaponStateActive::HandleReloadInput()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->ReloadingState);
	}
}

void UBOOMWeaponStateActive::HandleEquipping()
{
}


void UBOOMWeaponStateActive::HandleUnequipping()
{

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->UnequippingState);
	}
}
