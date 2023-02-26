// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BOOMUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMUserWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

};
