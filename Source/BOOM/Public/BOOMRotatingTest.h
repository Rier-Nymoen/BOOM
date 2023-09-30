// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ElectricInterface.h"
#include "BOOMRotatingTest.generated.h"

UCLASS()
class BOOM_API ABOOMRotatingTest : public AActor, public IElectricInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABOOMRotatingTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class UBOOMElectricRadiusComponent* ElectricRadiusComponent; //I should probably call the class Electric Conducting Component.

	virtual void OnConnectToPower() override;
	virtual void OnDisconnectFromPower() override;


	FTimerHandle TimerHandle_Rotate;
	bool bIsPowered;

	UFUNCTION()
	virtual void Spin();

};
