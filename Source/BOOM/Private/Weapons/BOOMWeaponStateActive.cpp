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
	//Since I am handling inputs here, maybe go to the next state, and
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Green, "Cast worked!!!");
		Weapon->GotoState(Weapon->FiringState);
	}
}
