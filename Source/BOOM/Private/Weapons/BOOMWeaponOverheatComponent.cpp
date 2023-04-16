// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BOOMWeaponOverheatComponent.h"

// Sets default values for this component's properties
UBOOMWeaponOverheatComponent::UBOOMWeaponOverheatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	HeatLimit = 100.0F;
	HeatRate = 10.0F;
	CooldownRate = 20.0F;
	CooldownStartTimeSeconds = 2.6F;


	// ...
}


// Called when the game starts
void UBOOMWeaponOverheatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//SEPARATE the overheat and type of fire component for modularity.
}


// Called every frame
void UBOOMWeaponOverheatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBOOMWeaponOverheatComponent::VentHeat()
{
}

