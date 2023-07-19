// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BOOMWeaponReloadComponent.h"

// Sets default values for this component's properties
UBOOMWeaponReloadComponent::UBOOMWeaponReloadComponent()
{

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

}

