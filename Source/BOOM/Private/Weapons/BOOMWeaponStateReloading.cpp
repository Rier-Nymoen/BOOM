// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeaponStateReloading.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/BOOMWeapon.h"

void UBOOMWeaponStateReloading::EnterState()
{

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon)
	{	
		// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0F, FColor(202,30, 106), "Reload::EnterState");

		if (!Weapon->CanReload())
		{
			Weapon->GotoStateActive();
			return;
		}
		//play anims etc
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, Weapon->ReloadDurationSeconds, FColor::Cyan, "Reloading");
		Weapon->GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, Weapon, &ABOOMWeapon::ReloadWeapon, Weapon->ReloadDurationSeconds, false);

	}
	
}

void UBOOMWeaponStateReloading::ExitState()
{
	CancelReload();
}

void UBOOMWeaponStateReloading::HandleFireInput()
{
	//disabled reload canceling for now
	//ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	//if (Weapon)
	//{
	//	Weapon->GotoState(Weapon->FiringState);
	//}
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
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, Weapon->ReloadDurationSeconds, FColor::Emerald, "Reload Ended");
		Weapon->GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}
}
