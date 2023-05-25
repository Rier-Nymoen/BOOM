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


	UFUNCTION()
		virtual void Fire();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:
	// Called every frame
	// 
	//Interface Implementations
	virtual void Interact() override;

	virtual void Interact(class ABOOMCharacter* MyCharacter) override;

	UFUNCTION()
		virtual void OnInteractionRangeEntered(class ABOOMCharacter* MyCharacter) override;

	UFUNCTION()
		virtual void OnInteractionRangeExited(class ABOOMCharacter* MyCharacter) override;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* WeaponMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* FireAction;

	//Maybe use operator overloading for different interaction scenarios? i know the character will be needed for some things.

};
