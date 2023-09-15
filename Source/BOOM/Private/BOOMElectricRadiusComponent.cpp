// Project BOOM


#include "BOOMElectricRadiusComponent.h"
#include "BOOMElectricSourceComponent.h"

UBOOMElectricRadiusComponent::UBOOMElectricRadiusComponent()
{

	SphereRadius = 250.F;
	SetCollisionProfileName("ElectricProfile");
	bHiddenInGame = true;
	ShapeColor = FColor(255, 0, 204);
}

void UBOOMElectricRadiusComponent::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UBOOMElectricRadiusComponent::OnSphereBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UBOOMElectricRadiusComponent::OnSphereEndOverlap);
}

void UBOOMElectricRadiusComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void UBOOMElectricRadiusComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
