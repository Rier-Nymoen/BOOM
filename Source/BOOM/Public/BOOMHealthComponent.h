// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BOOMHealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOOM_API UBOOMHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBOOMHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//figure out health and shield interactions

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	ROOT CLASS HOLDS SPECIFIC LOGIC

	MINIMUM SET OF REUSABLE CODE


	dont want to be super granular.
	*/
private:
		
};
