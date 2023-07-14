// Fill out your copyright notice in the Description page of Project Settings.


#include "BOOMHealthComponent.h"

// Sets default values for this component's properties
UBOOMHealthComponent::UBOOMHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	Health = 1000.0F;
	// ...
}


void UBOOMHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBOOMHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBOOMHealthComponent::AddHealth(int Amount)
{
}



