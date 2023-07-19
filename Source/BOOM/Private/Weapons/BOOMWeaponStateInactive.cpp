// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BOOMWeaponStateInactive.h"
#include "Weapons/BOOMWeapon.h"

void UBOOMWeaponStateInactive::EnterState()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor::Red, "InactiveState::EnterState");
}

void UBOOMWeaponStateInactive::ExitState()
{

}

void UBOOMWeaponStateInactive::HandleEquipping()
{


	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->EquippingState);
	}

}
