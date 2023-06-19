// Project BOOM


#include "Weapons/BOOMWeaponStateCharging.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "Weapons/BOOMWeapon.h"

UBOOMWeaponStateCharging::UBOOMWeaponStateCharging()
{
	ChargeRate = 50.0F;
	CurrentCharge = 0.0F;
	ChargedShotCost = 10.0F;
	UnchargedShotCost = 3.0F;
	ChargeThreshold = 100.0F;
	ChargeIncrementTimeSeconds = 0.91F;
}

void UBOOMWeaponStateCharging::BeginPlay()
{
	ChargeDelegate.BindUFunction(this, FName("AddCharge"), ChargeRate);

}

void UBOOMWeaponStateCharging::EnterState()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, ChargeIncrementTimeSeconds, FColor::Cyan, FString::SanitizeFloat(CurrentCharge));
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{             
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, ChargeIncrementTimeSeconds, FColor::Orange, "Timer set");

		Weapon->GetWorldTimerManager().SetTimer(TimerHandle_IncrementCharge, ChargeDelegate, ChargeIncrementTimeSeconds, true);
	}


}



void UBOOMWeaponStateCharging::ExitState()
{
	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		Weapon->GetWorldTimerManager().ClearTimer(TimerHandle_IncrementCharge);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, ChargeIncrementTimeSeconds, FColor::Orange, "Timer cleared");


		CurrentCharge = 0;

	}
}


void UBOOMWeaponStateCharging::HandleStopFiringInput()
{

	ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(GetOwner());
	if (Weapon)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, ChargeIncrementTimeSeconds, FColor::Cyan, "workl pls");

		if (CurrentCharge < ChargeThreshold)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Green, "Low charge shot");
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Green, "High charge shot");

		}


		Weapon->GotoState(Weapon->ActiveState);
	}
}

void UBOOMWeaponStateCharging::AddCharge(float Amount)
{
	CurrentCharge = FMath::Clamp(CurrentCharge + Amount, 0, ChargeThreshold);


	GEngine->AddOnScreenDebugMessage(INDEX_NONE, ChargeIncrementTimeSeconds, FColor::Cyan, "Current Charge is: " + FString::SanitizeFloat(CurrentCharge));



}
