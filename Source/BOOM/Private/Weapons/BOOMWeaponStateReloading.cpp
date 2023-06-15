// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeaponStateReloading.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/BOOMWeapon.h"

void UBOOMWeaponStateReloading::EnterState()
{

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon)
	{	
		Weapon->GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, Weapon, &ABOOMWeapon::ReloadWeapon, Weapon->ReloadDurationSeconds, false);
	}
	
}

void UBOOMWeaponStateReloading::ExitState()
{
	CancelReload();
}

void UBOOMWeaponStateReloading::HandleFireInput()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->FiringState);
	}
}

void UBOOMWeaponStateReloading::HandleEquipping()
{

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->EquippingState);
	}
}

void UBOOMWeaponStateReloading::HandleUnequipping()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->InactiveState);
	}
}

void UBOOMWeaponStateReloading::CancelReload()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}
}
