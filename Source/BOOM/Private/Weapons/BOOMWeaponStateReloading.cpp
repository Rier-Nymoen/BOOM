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
}

void UBOOMWeaponStateReloading::HandleFireInput()
{
	CancelReload();

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->FiringState);
	}
}

void UBOOMWeaponStateReloading::HandleEquipping()
{
	CancelReload();

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GotoState(Weapon->EquippingState);
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
