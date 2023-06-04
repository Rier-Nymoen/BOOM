// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BOOMWeaponStateInactive.h"

void UBOOMWeaponStateInactive::EnterState()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, FColor::Red, "InactiveState");
}
