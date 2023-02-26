// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BOOMUserWidget.h"
#include "BOOMPickUpPrompt.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMPickUpPrompt : public UBOOMUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
		void SetPromptText(FName PromptItemName) const;

	UFUNCTION()
		void SetPromptImage(class UTexture2D* NewPromptImage) const;


protected:

		UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* PromptText;


		UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* PromptItemImage;

	
};
