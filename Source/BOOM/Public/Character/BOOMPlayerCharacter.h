// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Character/BOOMCharacter.h"
#include "BOOMPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API ABOOMPlayerCharacter : public ABOOMCharacter
{
	GENERATED_BODY()

public:

	// ABOOMPlayerCharacter();
protected:

	//virtual void BeginPlay() override;

	//UFUNCTION()
	//void CheckPlayerLook();

	//virtual void Tick(float DeltaSeconds) override;

	/* 
	* Will wait as long as possible to exhaust baseclass before moving things to this class.
	* 
	*/
};
