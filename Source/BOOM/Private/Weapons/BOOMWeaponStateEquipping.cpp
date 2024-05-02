// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeaponStateEquipping.h"
#include "Weapons/BOOMWeapon.h"

void UBOOMWeaponStateEquipping::EnterState()
{	
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->OnEquip();
		Weapon->GotoState(Weapon->ActiveState);
	}
}

