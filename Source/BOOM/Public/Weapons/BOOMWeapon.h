// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "BOOMWeapon.generated.h"

class UBOOMPickUpComponent;
class ABOOMCharacter;
class UBOOMWeaponReloadComponent;

enum EWeaponState
{
	Firing,
	Reloading,
	Idle,

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

	//UPROPERTY(BlueprintReadWrite, Category = Component)
	//	UBOOMPickUpComponent* BOOMPickUp;
	UPROPERTY(EditAnywhere, Category = Component)
	UBOOMPickUpComponent* BOOMPickUp;

	UPROPERTY(EditAnywhere, Category = Component)
	UBOOMWeaponReloadComponent* ReloadComponent;

	UPROPERTY(EditAnywhere, Category = Name)
	FName Name = FName(TEXT("W1"));

private:
	/*Character holding weapon*/
	ABOOMCharacter* Character;

protected:

	UFUNCTION()
	virtual void Fire();

	UFUNCTION()
	virtual void ReloadWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds);


public:
	/*Interface Implementations*/
	virtual void Interact() override;

	virtual void Interact(class ABOOMCharacter* MyCharacter) override;

	UFUNCTION()
	virtual void OnInteractionRangeEntered(class ABOOMCharacter* MyCharacter) override;

	UFUNCTION()
	virtual void OnInteractionRangeExited(class ABOOMCharacter* MyCharacter) override;
	/*End of Interface Implementations*/

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* WeaponMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;
	
	/*experimental:*/ 
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;

	float WeaponDamage;
	//

protected:

	float HitscanRange;

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

	class UBOOMWeaponStateInactive* InactiveState;
	class UBOOMWeaponStateActive* ActiveState;
	class UBOOMWeaponState* CurrentState;
	class UBOOMWeaponStateEquipping* EquippingState;

	FTimerHandle TimerHandle_TestStateCanceling;

	virtual void GotoState(class UBOOMWeaponState* NewState);

	void TimerStart();

	void TimerCall();

	bool bCancelTimer;
};
