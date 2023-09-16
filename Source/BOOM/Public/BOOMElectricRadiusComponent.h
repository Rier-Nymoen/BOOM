// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Interfaces/ElectricInterface.h"
#include "BOOMElectricRadiusComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BOOM_API UBOOMElectricRadiusComponent : public USphereComponent, public IElectricInterface
{
	GENERATED_BODY()

public:

	UBOOMElectricRadiusComponent();
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnConnectToPower() override;
	virtual void OnDisconnectFromPower() override;

	UPROPERTY(VisibleAnywhere, Category = State);
	bool bIsPowered;

};
