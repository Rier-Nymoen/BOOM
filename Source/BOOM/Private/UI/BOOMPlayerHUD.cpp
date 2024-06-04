// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/BOOMPlayerHUD.h"
#include "Character/BOOMCharacter.h"
#include "Weapons/BOOMWeapon.h"

void UBOOMPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBOOMPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick( MyGeometry, InDeltaTime);
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (IsValid(OwningPawn))
	{
		ABOOMCharacter* OwningCharacter = Cast<ABOOMCharacter>(OwningPawn);
		if (OwningCharacter)
		{
			if (ABOOMWeapon* Weapon = Cast<ABOOMWeapon>(OwningCharacter->GetCurrentWeapon()))
			{
				GetWeaponInformationElement()->SetCurrentAmmoText(Weapon->GetCurrentAmmo());
				GetWeaponInformationElement()->SetWeaponNameText(Weapon->GetName());
				GetWeaponInformationElement()->SetReserveAmmoText(Weapon->GetCurrentAmmoReserves());
			}
		}
	}
}

UBOOMPickUpPrompt* UBOOMPlayerHUD::GetPickUpPromptElement()
{
	return PickUpPrompt;
}

UBOOMWeaponInformation* UBOOMPlayerHUD::GetWeaponInformationElement()
{
	return WeaponInformation;
}

UBOOMHealthInformation* UBOOMPlayerHUD::GetHealthInformationElement()
{
	return HealthInformation;
}
