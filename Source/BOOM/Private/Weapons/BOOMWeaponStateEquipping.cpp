// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeaponStateEquipping.h"
#include "Weapons/BOOMWeapon.h"

void UBOOMWeaponStateEquipping::EnterState()
{
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0F, FColor::Cyan, "Equipping::EnterState");
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{

		Weapon->OnEquip();
		Weapon->GotoState(Weapon->ActiveState);
	}
}

