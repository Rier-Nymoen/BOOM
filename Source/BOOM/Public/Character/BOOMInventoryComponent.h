// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BOOMInventoryComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOOM_API UBOOMInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBOOMInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION()
	bool HasNoWeapons();

	UFUNCTION()
	bool HasEmptyWeaponSlots();

	void AddWeapon(class ABOOMWeapon* NewWeapon);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABOOMGrenade> GrenadeType;

	int MaxWeaponSlots;
	int CurrentWeaponSlot;

	TArray<class ABOOMWeapon*> Weapons;


	//virtual bool CanEquip();
	//
	//virtual bool CanUnequip();
};
