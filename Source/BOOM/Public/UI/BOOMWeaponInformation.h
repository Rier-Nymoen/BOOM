// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "UI/BOOMUserWidget.h"
#include "BOOMWeaponInformation.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMWeaponInformation : public UBOOMUserWidget
{
	GENERATED_BODY()

	UBOOMWeaponInformation();

public:
	UFUNCTION()
		void SetCurrentAmmoText(int Value) const;

	UFUNCTION()
		void SetReserveAmmoText(int Value) const;


	UFUNCTION()
		void SetWeaponNameText(FName Name) const;

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* CurrentAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ReserveAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* WeaponNameText;
	
};
