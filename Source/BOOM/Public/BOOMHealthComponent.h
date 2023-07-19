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

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite)
	float Health;

	UPROPERTY(BlueprintReadWrite)
	float MaxHealth;


		//has very basic logic suited just for this component.

		//shield logic would have the same thing.

	//only has things to manipulate the object!!!!

	/*
	ROOT CLASS HOLDS SPECIFIC LOGIC

	MINIMUM SET OF REUSABLE CODE THAT IS GUARANTEED TO BE reused.


	dont want to be super granular.
	*/

	UFUNCTION()
	void AddHealth(int Amount);
private:
		
};
