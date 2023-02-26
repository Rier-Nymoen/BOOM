// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BOOMUserWidget.h"
#include "UI/BOOMPickUpPrompt.h"
#include "BOOMPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMPlayerHUD : public UBOOMUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UBOOMPickUpPrompt* PickUpPrompt;
	
};
