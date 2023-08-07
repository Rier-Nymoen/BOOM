// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BOOMWeaponStateUnequipping.h"
#include "Weapons/BOOMWeapon.h"

void UBOOMWeaponStateUnequipping::EnterState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->OnUnequip();
		Weapon->GotoState(Weapon->InactiveState);
	}
}

void UBOOMWeaponStateUnequipping::ExitState()
{
}

void UBOOMWeaponStateUnequipping::HandleFireInput()
{
}

void UBOOMWeaponStateUnequipping::HandleReloadInput()
{
}

void UBOOMWeaponStateUnequipping::HandleEquipping()
{
}
