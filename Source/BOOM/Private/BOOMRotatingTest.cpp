// Project BOOM


#include "BOOMRotatingTest.h"

#include "BOOMElectricRadiusComponent.h"

// Sets default values
ABOOMRotatingTest::ABOOMRotatingTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bIsPowered = false;

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
	if (!bIsPowered)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.F, FColor::Blue, "Powered");

		bIsPowered = true;
		//GetWorldTimerManager().SetTimer(TimerHandle_Rotate, this, &ABOOMRotatingTest::Spin, 0.01F, true);
	}

}

void ABOOMRotatingTest::OnDisconnectFromPower()
{
	bIsPowered = false;
	//GetWorldTimerManager().ClearTimer(TimerHandle_Rotate);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.F, FColor::Red, "Removed From Power");

}

void ABOOMRotatingTest::Spin()
{
	FRotator NewRotator(0,1.F,0);
	SetActorRotation(GetActorRotation() + NewRotator);
}

