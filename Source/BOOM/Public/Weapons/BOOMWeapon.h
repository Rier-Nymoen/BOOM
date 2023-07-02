// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "BOOMWeapon.generated.h"

class UBOOMPickUpComponent;
class ABOOMCharacter;
class UBOOMWeaponReloadComponent;

USTRUCT()
struct FFireInput
{
	GENERATED_BODY()
	bool bIsPendingFiring;
	//possibly additional information
};

UCLASS()
class BOOM_API ABOOMWeapon : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABOOMWeapon();
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Weapon1P;

	UPROPERTY(EditAnywhere, Category = Component)
	UBOOMPickUpComponent* BOOMPickUp;

	UPROPERTY(EditAnywhere, Category = Component)
	UBOOMWeaponReloadComponent* ReloadComponent;

	UPROPERTY(EditAnywhere, Category = Name)
	FName Name = FName(TEXT("W1"));

	/*Character holding weapon*/
	ABOOMCharacter* Character;

	FFireInput* FireInput;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds);

	float LastTimeFiredSeconds;

public:
	/*Interface Implementations*/

	virtual void Interact(class ABOOMCharacter* TargetCharacter) override;

	UFUNCTION()
	virtual void OnInteractionRangeEntered(class ABOOMCharacter* TargetCharacter) override;

	UFUNCTION()
	virtual void OnInteractionRangeExited(class ABOOMCharacter* TargetCharacter) override;
	/*End of Interface Implementations*/

	UFUNCTION()
	virtual void ReloadWeapon();
	
	UFUNCTION()
	virtual void HandleReloadInput();


	UFUNCTION()
	virtual void HandleFireInput();


	UFUNCTION()
	virtual void HandleStopFireInput();

	//Value should be a multiple of MagazineSize. Maximum Ammo Reserves.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int MaxAmmoReserves;
	
	//Ammo reserves are the amount of bullets you have, that you can reload into the weapon. (Like Halo's system, not meant to be realistic)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int CurrentAmmoReserves;

	//Cost of firing a shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int AmmoCost;

	//Max rounds held by a magazine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int MagazineSize;

	//Rounds in the magazine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int CurrentAmmo;

	UFUNCTION()
	virtual void AddAmmo(int Amount);

	UFUNCTION()
	virtual bool HasAmmo();

	virtual void HandleEquipping();

	virtual void HandleUnequipping();

	virtual FRotator CalculateSpread(FRotator PlayerLookRotation);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;

	UFUNCTION()
	virtual void Fire();

	UFUNCTION()
	virtual bool IsIntendingToRefire();

	class ABOOMCharacter* GetCharacter();

	//virtual void FireHitscan();

	//virtual void FireProjectile();

	float WeaponDamage;
	
	//
	virtual void GotoState(class UBOOMWeaponState* NewState);


	class UBOOMWeaponState* InactiveState;
	class UBOOMWeaponState* ActiveState;
	class UBOOMWeaponState* CurrentState;
	class UBOOMWeaponState* EquippingState;
	class UBOOMWeaponState* FiringState;
	class UBOOMWeaponState* ReloadingState;
	class UBOOMWeaponState* UnequippingState;

	//The time it takes to reload
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ReloadDurationSeconds;

protected:
	float HitscanRange;

	float FireRateSeconds;
public:
	
	UFUNCTION()
	float GetFireRateSeconds();

	
	////consider moving to the "firing state" class
	UFUNCTION()
	float GetLastTimeFiredSeconds();

	bool IsReadyToFire();

	virtual void HandleBeingDropped();
	
};
