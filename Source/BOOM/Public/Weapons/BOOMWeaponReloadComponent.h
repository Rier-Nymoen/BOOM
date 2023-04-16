// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BOOMWeaponReloadComponent.generated.h"




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOOM_API UBOOMWeaponReloadComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBOOMWeaponReloadComponent();
	friend class ABOOMWeapon;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:	


	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Value should be a multiple of MagazineSize. Maximum Ammo Reserves.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int MaxAmmoReserves;

	//Ammo reserves are the amount of bullets you have, that you can reload into the weapon. (Like Halo's system, not meant to be realistic)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int CurrentAmmoReserves;

	//Rounds held by magazine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int AmmoCost;

	//Rounds held by magazine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int MagazineSize;

	//Rounds in the magazine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int CurrentAmmo;

	//The time it takes to reload
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float ReloadDurationSeconds;

	//Paraphrased: Unique handle to distinguish timers with the same delegates.
	FTimerHandle TimerHandle_ReloadWeapon;


	bool bCanFire = true;

	bool bIsReloading = false;
	
	virtual void ReloadWeapon();
	

	
};
