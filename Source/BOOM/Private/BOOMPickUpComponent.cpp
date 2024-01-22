// Fill out your copyright notice in the Description page of Project Settings.


#include "BOOMPickUpComponent.h"
#include "Character/BOOMCharacter.h"
// Sets default values for this component's properties
UBOOMPickUpComponent::UBOOMPickUpComponent()
{
	//SphereRadius = PickUpRadius Should be based around weapon size maybe? Arbitrary value for now;
	SphereRadius = 150.F;
	bHiddenInGame = false;
	ShapeColor = FColor(3, 30, 255);

}

void UBOOMPickUpComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// Called when the game starts
void UBOOMPickUpComponent::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UBOOMPickUpComponent::OnSphereBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UBOOMPickUpComponent::OnSphereEndOverlap);


}

void UBOOMPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}


