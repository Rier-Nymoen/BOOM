// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Weapons/BOOMWeapon.h"
#include "Weapons/BOOMWeaponStateReloading.h"

void UBOOMWeaponStateReloading::EnterState()
{

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon)
	{	
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Cyan, "Reloading state");
		Weapon->GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, Weapon, &ABOOMWeapon::ReloadWeapon, 2.2F, false);
	}
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Cyan, "Should run after attempting to cancel reload");
	
}

void UBOOMWeaponStateReloading::ExitState()
{
}

/*
Redundancy if multiple states handle inputs the same exact way possibly. I mean states change based on input so idk.
maybe just having gotos based on first and previous. not sure.

*/
void UBOOMWeaponStateReloading::HandleFireInput()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Cyan, "Trying to cancel reload");

		Weapon->GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
		Weapon->GotoState(Weapon->FiringState);
	}
}