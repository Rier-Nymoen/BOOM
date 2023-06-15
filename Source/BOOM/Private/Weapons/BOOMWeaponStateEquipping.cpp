// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeaponStateEquipping.h"
#include "Weapons/BOOMWeapon.h"

void UBOOMWeaponStateEquipping::EnterState()
{
	//
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.4F, FColor::Cyan, "Equipping Weapon");
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->ActiveState);
	}
}

