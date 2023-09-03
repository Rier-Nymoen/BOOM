// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BOOMWeaponStateFiring.h"
#include "BOOMWeaponStateCharging.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMWeaponStateCharging : public UBOOMWeaponStateFiring
{
	GENERATED_BODY()
public:

	UBOOMWeaponStateCharging();

	virtual void BeginPlay() override;

	virtual void EnterState() override;

	virtual void ExitState() override;

protected:
	
	UPROPERTY()
	FTimerHandle TimerHandle_OnOvercharged;

	bool bFiresOnOvercharged;

	bool bIsOvercharged;
	UFUNCTION()
	virtual void OnOvercharged();

	UFUNCTION()
	virtual void HandleStopFiringInput() override;
	
};
