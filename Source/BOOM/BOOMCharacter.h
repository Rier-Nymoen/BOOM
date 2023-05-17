// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Engine/DataTable.h"

#include "BOOMCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

//want to use Data table since its stored on disk
USTRUCT(BlueprintType)
struct FItemInformation : public FTableRowBase
{
	GENERATED_BODY()
public:
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UTexture2D* ItemImage;
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString Description;
protected:

private:
	
};

UCLASS(config = Game)
class ABOOMCharacter : public ACharacter 
{
	GENERATED_BODY()

		/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

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


public:
	ABOOMCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds);

	UFUNCTION()
	void OnCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
		bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
		bool GetHasRifle();

	bool bIsOverlappingWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DataTable)
		class UDataTable* WeaponTable;



protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void SwapWeapon(const FInputActionValue& Value);

	void Fire(const FInputActionValue& Value);
	
	void Interact(const FInputActionValue& Value);



	UPROPERTY()
	class ABOOMWeapon* CurrentWeapon;
	UPROPERTY()
	class ABOOMWeapon* PrimaryWeapon;
	UPROPERTY()
	class ABOOMWeapon* SecondaryWeapon;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface
	UFUNCTION()
	void SetCurrentWeapon(class ABOOMWeapon* Weapon);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		TSubclassOf<class UBOOMPlayerHUD> PlayerHUDClass;

	UPROPERTY()
		class UBOOMPlayerHUD* PlayerHUD;
	UPROPERTY()
	TSet< AActor* > OverlappedActors;

	UFUNCTION()
		class UBOOMPlayerHUD* GetPlayerHUD();


	UFUNCTION()
		class ABOOMWeapon* GetCurrentWeapon();

	UFUNCTION()
		class ABOOMWeapon* GetPrimaryWeapon();


	UFUNCTION()
		class ABOOMWeapon* GetSecondaryWeapon();

	UFUNCTION()
		void  SetPrimaryWeapon(class ABOOMWeapon* Weapon);


	UFUNCTION()
		void  SetSecondaryWeapon(class ABOOMWeapon* Weapon);


	UFUNCTION()
		void EquipWeapon(class ABOOMWeapon* Weapon);

	//Should change to lookinteraction range, I think the ranges of something you're looking at and standing on may need to be different logically.
	UPROPERTY()
	float InteractionRange;

	//Item currently interacting with
	UPROPERTY()
	AActor* HighlightedActor;
};