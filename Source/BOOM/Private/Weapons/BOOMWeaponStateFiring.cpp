// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeaponStateFiring.h"
#include "Weapons/BOOMWeaponStateActive.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/BOOMWeapon.h"
#include "BOOM/BOOMCharacter.h"
#include "CoreMinimal.h"

/*
I believe refire functionality should reside in the Firing state. Otherwise during state transition, it would be difficult.
*/
UBOOMWeaponStateFiring::UBOOMWeaponStateFiring()
{
	//FireRateSeconds = 2.0F;
	//FireRateSeconds = 0.066F;
}

void UBOOMWeaponStateFiring::BeginPlay()
{
}

void UBOOMWeaponStateFiring::EnterState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	//Upon entering the state, fire	and have refire timer

	if (Weapon != nullptr && Weapon->IsReadyToFire())
	{
			
			Weapon->Fire();
			Weapon->GetWorldTimerManager().SetTimer(TimerHandle_RefireTimer, this, &UBOOMWeaponStateFiring::CheckRefireTimer, Weapon->GetFireRateSeconds(), true);
		
	}
}

void UBOOMWeaponStateFiring::ExitState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());




	if (Weapon != nullptr)
	{

		Weapon->GetWorldTimerManager().ClearTimer(TimerHandle_RefireTimer);


	}

}



void UBOOMWeaponStateFiring::HandleUnequipping()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Cyan, "stateended");

		Weapon->GotoState(Weapon->InactiveState);
	}

}

void UBOOMWeaponStateFiring::HandleReloadInput()
{


	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());

	if (Weapon != nullptr)
	{
		Weapon->GotoState(Weapon->ReloadingState);
	}
}



//if we dont check refiring from the timer, then if we cant refire and when holding m1, it wont refire when it should! Therefore we need to have a refire check timer.

void UBOOMWeaponStateFiring::CheckRefireTimer()
{
	//@todo add weapon says its okay to keep firing
	//if weapon and if weapon says its okay to keep firing.. fire.
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon != nullptr)
	{
		
		if (Weapon->IsIntendingToRefire()) //If true, function causes weapon state to change
		{
			Weapon->Fire();
			return;
		}

	}

}

