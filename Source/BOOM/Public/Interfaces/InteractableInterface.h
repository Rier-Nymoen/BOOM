// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BOOM_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//UFUNCTIONS cannot be overloaded, if I want to do this with blueprints, I may need to change my function signatures.
	//be careful about if it gets garbage collected? ufunc needed for delegates and timelines
	//https ://forums.unrealengine.com/t/what-does-ufunction-and-uproperty-without-any-arguments-do/42469/2


	virtual void Interact(class ABOOMCharacter* MyCharacter) = 0;

	virtual void OnInteractionRangeEntered(class ABOOMCharacter* MyCharacter) = 0;
	virtual void OnInteractionRangeExited(class ABOOMCharacter* MyCharacter) = 0;

};
