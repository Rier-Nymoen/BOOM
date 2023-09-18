// Project BOOM


#include "BOOMRotatingTest.h"

#include "BOOMElectricRadiusComponent.h"

// Sets default values
ABOOMRotatingTest::ABOOMRotatingTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABOOMRotatingTest::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABOOMRotatingTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABOOMRotatingTest::OnConnectToPower()
{
	GetWorldTimerManager().SetTimer(TimerHandle_Rotate, this, &ABOOMRotatingTest::Spin, 0.1F, true);

}

void ABOOMRotatingTest::OnDisconnectFromPower()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Rotate);

}

void ABOOMRotatingTest::Spin()
{
	FRotator NewRotator(0,30.F,0);
	SetActorRotation(GetActorRotation() + NewRotator);
}

