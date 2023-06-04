// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeaponStateFiring.h"
#include "Weapons/BOOMWeaponStateActive.h"

#include "Weapons/BOOMWeapon.h"

void UBOOMWeaponStateFiring::EnterState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->Fire();
	}
	ExitState();
}

void UBOOMWeaponStateFiring::ExitState()
{

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.4F, FColor::Cyan, "Firing exitstate");

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->ActiveState);
	}
}

void UBOOMWeaponStateFiring::HandleFireInput()
{
}
