// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BOOMWeaponStateActive.h"
#include "Weapons/BOOMWeaponStateFiring.h"
#include "Weapons/BOOMWeapon.h"
#include "BOOM/BOOMCharacter.h"

void UBOOMWeaponStateActive::EnterState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{

		if (Weapon->GetCharacter()->bIsPendingFiring && Weapon->CurrentAmmo > 0)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Red, "Active State Held Input on State Entrance");


			Weapon->GotoState(Weapon->FiringState);
		}

	}
}


void UBOOMWeaponStateActive::ExitState()
{

}

void UBOOMWeaponStateActive::HandleFireInput()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon->CurrentAmmo > 0)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Red, "Active State Handle Fire Input");

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
