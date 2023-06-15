// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeaponStateFiring.h"
#include "Weapons/BOOMWeaponStateActive.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/BOOMWeapon.h"

UBOOMWeaponStateFiring::UBOOMWeaponStateFiring()
{
	FireCooldownSeconds = 0.5F;
	bIsOnFireCooldown = false;
}

void UBOOMWeaponStateFiring::BeginPlay()
{
}

void UBOOMWeaponStateFiring::EnterState()
{

	if (!bIsOnFireCooldown)
	{
		ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
		if (Weapon != nullptr)
		{
			Weapon->GetWorldTimerManager().SetTimer(TimerHandle_FireCooldown, this, &UBOOMWeaponStateFiring::EndFireCooldown, FireCooldownSeconds, false);
			Weapon->Fire(); 
			Weapon->GotoState(Weapon->ActiveState);

		}
		
		
	}
	

}

void UBOOMWeaponStateFiring::ExitState()
{

}

void UBOOMWeaponStateFiring::HandleFireInput()
{
}

void UBOOMWeaponStateFiring::HandleUnequipping()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon != nullptr)
	{
		Weapon->GotoState(Weapon->InactiveState);
	}

}

void UBOOMWeaponStateFiring::HandleStopFiringInput()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon != nullptr)
	{
		Weapon->GotoState(Weapon->ActiveState);
	}
}



void UBOOMWeaponStateFiring::EndFireCooldown()
{
	bIsOnFireCooldown = false;
}

