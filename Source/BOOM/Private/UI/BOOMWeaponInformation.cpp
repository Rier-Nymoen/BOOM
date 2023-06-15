// Project BOOM


#include "UI/BOOMWeaponInformation.h"
#include "Components/TextBlock.h"

void UBOOMWeaponInformation::SetCurrentAmmoText(int Value) const
{
	CurrentAmmoText->SetText(FText::AsNumber(Value));
}

void UBOOMWeaponInformation::SetReserveAmmoText(int Value) const
{
	ReserveAmmoText->SetText(FText::AsNumber(Value));
}

void UBOOMWeaponInformation::SetWeaponNameText(FName Name) const
{
	WeaponNameText->SetText(FText::FromName(Name));
}
