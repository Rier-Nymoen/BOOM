// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Engine/DataTable.h"
#include "GameplayEffectTypes.h"

//
#include "AbilitySystemInterface.h"

#include "BOOMCharacter.generated.h"
class UBOOMCharacterMovementComponent;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class UAbilitySystemComponent;

//want to use Data table since its stored on disk. 
USTRUCT(BlueprintType)
struct FItemInformation : public FTableRowBase
{
	GENERATED_BODY()
public:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UTexture2D* ItemImage;
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString Description;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(config = Game)
class ABOOMCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	///*3rd person view, seen by other observers*/
	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//USkeletalMeshComponent* Mesh3P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

public:
	ABOOMCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketNameGripPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketNameHolsterPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketNameGripPoint3P;

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds);
	UFUNCTION()
	void OnCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnCharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, Category = "Character")
	UBOOMCharacterMovementComponent* BOOMCharacterMovementComp;

private:
	UPROPERTY()
	TArray< AActor* > OverlappedActors;

	UPROPERTY()
	int Overlaps;
public:
	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();
	 
	UPROPERTY()
	bool bIsOverlappingWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable)
	class UDataTable* WeaponTable;

	void StartFire();
	UFUNCTION()
	void StopFire();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void SwapWeapon(const FInputActionValue& Value);

	void StartCrouch(const FInputActionValue& Value);
	void EndCrouch(const FInputActionValue& Value);

	virtual void Jump();

	//void StartFire(const FInputActionValue& Value);

	UFUNCTION()
	void Reload();

	UFUNCTION()
	virtual void Fire();


	void Interact(const FInputActionValue& Value);

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	/** Weapon Swap Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* WeaponSwapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* StopFireAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* GrenadeThrowAction;

protected:

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	UPROPERTY()
	class UBOOMAttributeSetBase* AttributeSetBase;

	UFUNCTION()
	void DropCurrentWeapon();

	bool bIsPendingFiring;

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<class UBOOMPlayerHUD> PlayerHUDClass;

	UPROPERTY()
	class UBOOMPlayerHUD* PlayerHUD;

	UFUNCTION()
	class UBOOMPlayerHUD* GetPlayerHUD();

	//The Look-Range the player can detect interactable objects.
	UPROPERTY()
	float InteractionRange;

	UFUNCTION()
	AActor* GetNearestInteractable();

	UFUNCTION()
	void CheckPlayerLook();

	//Prioritized item currently being interacted with
	UPROPERTY()
	AActor* HighlightedActor;

public:
	int MaxWeaponsEquipped;

	UPROPERTY()
	int CurrentWeaponSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta=(ArrayClamp="2"))
	TArray<TSubclassOf<ABOOMWeapon>> WeaponsToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABOOMGrenade> GrenadeType;

	void SpawnWeapons();

	TArray<class ABOOMWeapon*> Weapons;

	class ABOOMWeapon* Weapon;

	void ThrowGrenade();

	UFUNCTION()
	virtual void EquipWeapon(ABOOMWeapon* TargetWeapon);

	UFUNCTION()
	bool HasNoWeapons();

	UFUNCTION()
	bool HasEmptyWeaponSlots();

	UFUNCTION()
	float GetHealth();

	UFUNCTION()
	bool IsAlive();


protected:

	UPROPERTY()
	class UBOOMHealthComponent* HealthComponent;
	//experimenting, remove its specifier potentialy
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UFUNCTION()
	void HandleDeath();

	virtual void ThrowInventory();
	
	//move into a Health Component eventually
	FTimerHandle TimerHandle_ShieldRechargeDelay;
	FTimerHandle TimerHandle_ShieldRecharge;

	float ShieldFullRechargeDurationSeconds;

	float ShieldRechargeInterpSeconds;
	float ShieldRechargeDelaySeconds;

	UFUNCTION()
	virtual void InterpShieldRegen();
	UFUNCTION()
	virtual void RegenerateShields();

	virtual void HandleShieldStrengthChanged(const FOnAttributeChangeData& Data);

	void SetupCharacterAbilities();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Abilities")
	TArray<TSubclassOf<class UBOOMGameplayAbility>> CharacterAbilities;


	//virtual void HandleHealthChanged(const FOnAttributeChangeData& Data);
	UFUNCTION()
	virtual void HandleHealthChanged(float OldValue, float NewValue);
	
	bool bIsDead;

	UFUNCTION()
	void Zoom();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Input)
	bool bIsFocalLengthScalingEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* DeathMontage;


};