// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BOOMUserWidget.h"
#include "UI/BOOMPickUpPrompt.h"
#include "UI/BOOMWeaponInformation.h"
#include "BOOMPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMPlayerHUD : public UBOOMUserWidget
{
	GENERATED_BODY()

public:
	class UBOOMPickUpPrompt* GetPickUpPromptElement();

	class UBOOMWeaponInformation* GetWeaponInformationElement();


protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UBOOMPickUpPrompt* PickUpPrompt;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UBOOMWeaponInformation* WeaponInformation;

};
