// Project BOOM


#include "Character/BOOMInventoryComponent.h"

// Sets default values for this component's properties
UBOOMInventoryComponent::UBOOMInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	CurrentWeaponSlot = 0;
	MaxWeaponSlots = 2;
	// ...
}


// Called when the game starts
void UBOOMInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBOOMInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UBOOMInventoryComponent::HasNoWeapons()
{
	return false;
}

bool UBOOMInventoryComponent::HasEmptyWeaponSlots()
{
	return false;
}

void UBOOMInventoryComponent::AddWeapon(ABOOMWeapon* NewWeapon)
{
	Weapons.Add(NewWeapon);
	/*If the inventory manipulates the state of the weapon, it doesn't seem right.*/
}

