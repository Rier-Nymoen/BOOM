// Project BOOM


#include "BOOMElectricArc.h"
#include "Components/BoxComponent.h"
// Sets default values
ABOOMElectricArc::ABOOMElectricArc()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	DamageBox = CreateDefaultSubobject<UBoxComponent>("DamageBox");
	DamageBox->SetupAttachment(RootComponent);
	DamageBox->SetHiddenInGame(true);
	DamageBox->ShapeColor = FColor(10, 70, 255);
	DamageBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

UBoxComponent* ABOOMElectricArc::GetBoxComponent()
{
	return DamageBox;
}

void ABOOMElectricArc::InitActor()
{
	DamageBox->SetHiddenInGame(false);
	DamageBox->ShapeColor = FColor(10, 70, 255);
}

void ABOOMElectricArc::ReturnActorToPool()
{
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

