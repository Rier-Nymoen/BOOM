// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AICombatPropertiesComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOOM_API UAICombatPropertiesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAICombatPropertiesComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//The rate at which we send a firing input per seconds. If its zero, we hold down the firing input until we need to stop shooting.
	UPROPERTY(EditAnywhere)
	float RateOfFiringInputSeconds;

	UPROPERTY(EditAnywhere)
	float TargetTrackingAccuracy;

	UPROPERTY(EditAnywhere)
	float TargetLeadingAccuracy;

	UPROPERTY(EditAnywhere)
	float BurstDurationSeconds;

	UPROPERTY(EditAnywhere)
	float TimeBetweenBursts;

		
};
