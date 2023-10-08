// Project BOOM


#include "BOOMElectricArc.h"
#include "Components/BoxComponent.h"
// Sets default values
ABOOMElectricArc::ABOOMElectricArc()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	DamageBox = CreateDefaultSubobject<UBoxComponent>("DamageBox");
	DamageBox->SetHiddenInGame(false);

}

UBoxComponent* ABOOMElectricArc::GetBoxComponent()
{
	return DamageBox;
}

// Called when the game starts or when spawned
void ABOOMElectricArc::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ABOOMElectricArc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

