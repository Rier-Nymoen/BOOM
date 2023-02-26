// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BOOMPickUpPrompt.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"



void UBOOMPickUpPrompt::SetPromptText(FName PromptItemName) const
{
	PromptText->SetText(FText::FromName(PromptItemName));
}

void UBOOMPickUpPrompt::SetPromptImage(UTexture2D* NewPromptImage) const
{
	PromptItemImage->SetBrushFromTexture(NewPromptImage, true);
}

