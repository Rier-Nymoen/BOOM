// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "BOOMWeaponStateReloading.h"
#include "Components/ActorComponent.h"
#include "BOOMWeaponStateFeedReloading.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BOOM_API UBOOMWeaponStateFeedReloading : public UBOOMWeaponStateReloading
{
	GENERATED_BODY()

	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleFireInput() override;

public:

protected:

	UPROPERTY()
	FTimerHandle TimerHandle_FeedReload;

	FTimerHandle TimerHandle_StartFeedReload;

	virtual void CheckReloadTimer();

};
