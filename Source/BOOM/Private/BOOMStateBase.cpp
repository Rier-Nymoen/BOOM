// Fill out your copyright notice in the Description page of Project Settings.


#include "BOOMStateBase.h"

// Sets default values for this component's properties
UBOOMStateBase::UBOOMStateBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UBOOMStateBase::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBOOMStateBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBOOMStateBase::EnterState()
{
}

void UBOOMStateBase::ExitState()
{
}

void UBOOMStateBase::HandleInput()
{
}

