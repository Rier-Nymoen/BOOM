// Project BOOM


#include "Weapons/BOOMWeaponStateCharging.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "Weapons/BOOMWeapon.h"

UBOOMWeaponStateCharging::UBOOMWeaponStateCharging()
{
	/*
	 *
	 *
	 * An interface 
	 */
	bIsOvercharged = false;
	bFiresOnOvercharged = true;
}

void UBOOMWeaponStateCharging::BeginPlay()
{

}

void UBOOMWeaponStateCharging::EnterState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GetWorldTimerManager().SetTimer(TimerHandle_OnOvercharged,  this, &UBOOMWeaponStateCharging::OnOvercharged, 2.F, false);
	}

}

void UBOOMWeaponStateCharging::ExitState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		
		bIsOvercharged = false;
	}
}

void UBOOMWeaponStateCharging::OnOvercharged()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		bIsOvercharged = true;
		if(bFiresOnOvercharged)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.F, FColor::Blue, "Fired on overchargeD");

			Weapon->Fire();

			//Expand upon allowing other firing states to be called as a result. Ex: Weapon that charges but releases a burst.
			Weapon->GotoStateActive();
		}
	}
}

void UBOOMWeaponStateCharging::HandleStopFiringInput()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		bIsOvercharged ? Weapon->Fire() : Weapon->ZoomIn();  /*Could have this go to a different firing state entirely, but firemodes and firing states need to be incorporated together.*/
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.F, FColor::Cyan, "Fired or zoomed in on stop firing");
	}
	Weapon->GotoStateActive();
}

