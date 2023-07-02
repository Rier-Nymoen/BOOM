// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BOOMWeaponOverheatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOOM_API UBOOMWeaponOverheatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBOOMWeaponOverheatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:

	UPROPERTY()
	float HeatRate;

	UPROPERTY()
	float CooldownRate;

	UPROPERTY()
	float HeatLimit;

	UPROPERTY()
	float CooldownStartTimeSeconds;

	UFUNCTION()
	void VentHeat();
	
	//weapon's overheat animation.
};
