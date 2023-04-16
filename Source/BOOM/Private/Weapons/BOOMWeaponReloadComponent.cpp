// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BOOMWeaponReloadComponent.h"

// Sets default values for this component's properties
UBOOMWeaponReloadComponent::UBOOMWeaponReloadComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//Set max ammo reserves
	MaxAmmoReserves = 12;
	//Player will spawn in with the max ammo reserves at the start of the game
	CurrentAmmoReserves = MaxAmmoReserves;
	AmmoCost = 1;

	//Should be able to fire initially

	//Set size of magazine
	MagazineSize = 10;

	//Player will spawn with a full magazine
	CurrentAmmo = MagazineSize;

	//Time it takes for a magazine to be reloaded into the weapon
	ReloadDurationSeconds = 2.2f;
	// ...
}


// Called when the game starts
void UBOOMWeaponReloadComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBOOMWeaponReloadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBOOMWeaponReloadComponent::ReloadWeapon()
{
	//if we have Reserve Ammo to reload the weapon, then we can do this
	if (CurrentAmmoReserves > 0)
	{
		//How much ammo is missing from the mags ammo capacity
		int BulletDifference = (MagazineSize - CurrentAmmo);

		if (CurrentAmmoReserves >= MagazineSize)
		{
			CurrentAmmo += BulletDifference;
			CurrentAmmoReserves -= BulletDifference;
		}
		else
		{
			CurrentAmmo += CurrentAmmoReserves;
			CurrentAmmoReserves = 0;
		}
		bCanFire = true;
	}
	bIsReloading = false;
}

