// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/InteractableInterface.h"
#include "BOOMWeapon.generated.h"

class UBOOMPickUpComponent;
class ABOOMCharacter;
class UAbilitySystemComponent;

//class UBOOMWeaponReloadComponent;

UENUM()
enum class EZoom : int8
{
	Not_Zoomed,
	Zoomed,
};


UENUM()
enum class EFiringSource : int8
{
	Camera,
	WeaponMuzzle,
	OwnerCenter
};

UENUM()
enum class ESpreadMode : int8
{
	HeatBasedSpread,
	RandomSpread
};


UCLASS()
class BOOM_API ABOOMWeapon : public AActor, public IInteractableInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	ABOOMWeapon();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds);

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Weapon1P;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Weapon3P;
public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	USkeletalMeshComponent* GetMeshWeapon1P() const { return Weapon1P; }
	USkeletalMeshComponent* GetMeshWeapon3P() const { return Weapon3P; }

	UPROPERTY(VisibleAnywhere, Category = Component)
	UBOOMPickUpComponent* BOOMPickUp;

	UPROPERTY(EditAnywhere, Category = Name)
	FName Name = FName(TEXT("Default Weapon Name"));

	/*Character holding weapon*/
	ABOOMCharacter* Character;


protected:
	float LastTimeFiredSeconds;

	EZoom ZoomMode;

	UPROPERTY(EditAnywhere)
	EFiringSource FiringSource;

	UPROPERTY(EditAnywhere)
	ESpreadMode SpreadMode;

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
	virtual void FeedReloadWeapon();

	UFUNCTION()
	virtual void HandleReloadInput();
	
	UFUNCTION()
	virtual void HandleFireInput();

	UFUNCTION()
	virtual void HandleSingleFireInput();
	
	UFUNCTION()
	virtual void HandleStopFireInput();

protected:
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int BulletsPerShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float HitscanRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRateSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInstance* ImpactDecal;

	bool bIsPendingFiring;

public:
	bool IsPendingFiring() const { return bIsPendingFiring; }

	UFUNCTION(BlueprintCallable)
	int GetMaxAmmoReserves() const  { return MaxAmmoReserves; }

	UFUNCTION(BlueprintCallable)
	int GetCurrentAmmoReserves() const { return CurrentAmmoReserves; }

	UFUNCTION(BlueprintCallable)
	int GetMagazineSize() const { return MagazineSize; }

	UFUNCTION(BlueprintCallable)
	int GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintCallable)
	float GetHitscanRange() const { return HitscanRange; }

	UFUNCTION()
	virtual void AddAmmo(int Amount);
	
	UFUNCTION(BlueprintCallable)
	virtual bool HasAmmo();

	virtual void HandleEquipping();

	virtual void HandleUnequipping();

	UFUNCTION()
	virtual void Fire();

	UFUNCTION()
	virtual bool IsIntendingToRefire();

	class ABOOMCharacter* GetCharacter();
protected:
	virtual void FireHitscan();

	virtual void FireProjectile();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<class ABOOMProjectile> Projectile;
	//
public:
	virtual void GotoState(class UBOOMWeaponState* NewState);

	class UBOOMWeaponState* InactiveState;
	class UBOOMWeaponState* ActiveState;
	UPROPERTY(VisibleAnywhere)
	class UBOOMWeaponState* CurrentState;
	UPROPERTY(VisibleAnywhere)
	class UBOOMWeaponState* EquippingState;
	UPROPERTY(VisibleAnywhere)
	class UBOOMWeaponState* FiringState;
	UPROPERTY(VisibleAnywhere)
	class UBOOMWeaponState* ReloadingState;
	UPROPERTY(VisibleAnywhere)
	class UBOOMWeaponState* UnequippingState;

	//The time it takes to reload
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ReloadDurationSeconds;

	UFUNCTION()
	void GotoStateEquipping();

	UFUNCTION()
	void GotoStateActive();

	UFUNCTION()
	void GotoStateInactive();
	
	UFUNCTION()
	float GetFireRateSeconds();

	////consider moving to the "firing state" class
	UFUNCTION()
	float GetLastTimeFiredSeconds();

	bool IsReadyToFire();
	//
	virtual void HandleBeingDropped();

	virtual void DisableCollision();

	virtual bool CanReload();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Overrides)
	bool bVisualizeHitscanTraces;
	
	UPROPERTY(BlueprintReadWrite, Category = Weapon)

	//Maps current heat to added heat value for consecutive shots
	struct FRuntimeFloatCurve HeatIncreaseCurve;

	UPROPERTY(BlueprintReadWrite, Category = Weapon)
	struct FRuntimeFloatCurve HeatCooldownCurve;

	virtual void Cooldown();
	
	void InterpRecoil();


	FTimerHandle TimerHandle_WeaponCooldown;

	virtual void Zoom();
	
	virtual void ZoomOut();

	virtual void ZoomIn();

	virtual void OnEquip();

	virtual void OnUnequip();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* StopFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomAction;


	//Maps heat level to bullet spread angle

	//Affects how likely shots are to group towards the center. From the center, a random value is picked and raised to the power of SpreadGroupingExponent
	UPROPERTY(EditAnywhere, Category = Spread)
	float SpreadGroupingExponent;

	UPROPERTY(EditAnywhere, Category = GameplayEffects)
	TSubclassOf<class UGameplayEffect> DamageEffect;	
protected:
	float CurrentHeat;

	//type for defining curves for object in editor
	UPROPERTY(EditAnywhere, Category = Spread)
	struct FRuntimeFloatCurve HeatToSpreadCurve;

	UPROPERTY(EditAnywhere, Category = Spread)
	struct FRuntimeFloatCurve HeatToCooldownCurve;

	UPROPERTY(EditAnywhere, Category = Spread)
	struct FRuntimeFloatCurve HeatToHeatIncreaseCurve;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float WeaponCoolingStartSeconds;

	FVector CalculateBulletSpreadDir(FRotator StartRot);

	UPROPERTY(EditAnywhere, Category = Spread)
	bool bUseHeatBasedSpreadRecoil;

	UPROPERTY(EditAnywhere, Category = Spread)
	float MaxSpreadAngle;


	UPROPERTY(EditAnywhere, Category = Spread)
	float MinSpreadAngle;

	float CurrentSpreadAngle;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
	TSubclassOf<class UCameraShakeBase> FiringCameraShakeClass;
	/*RECOIL*/

	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	TArray<FRotator> RecoilPattern;

	int RecoilIndex;

	UPROPERTY(EditAnywhere, Category = Spread)
	struct FRuntimeFloatCurve HeatToRecoilCurve;

	FRotator TargetRotation;
	FRotator CurrentRotation;

	float VerticalRecoil;

	UPROPERTY(EditAnywhere, Category = Debug)
	float DebugInterpSpeedRecoil;

	UPROPERTY(EditAnywhere, Category = Debug)
	float DebugInterpSpeedRecovery;

	FRotator FirstShotRotation;

	//UPROPERTY(EditDefaultsOnly, Category = Effects)
	//class UNiagraSystem* BulletTraceFX;


};
