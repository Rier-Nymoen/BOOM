// Project BOOM


#include "BOOMElectricRadiusComponent.h"

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
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0F, FColor::Cyan, "Electricity connection");
}

void UBOOMElectricRadiusComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0F, FColor::Orange, "Electricity disconnection");

}
