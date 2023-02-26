// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BOOMPickUpComponent.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBOOMOnPickUp, ABOOMCharacter*, CharacterPickingUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreate);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BOOM_API UBOOMPickUpComponent : public USphereComponent
{
	GENERATED_BODY()
public:
	UBOOMPickUpComponent();

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FBOOMOnPickUp OnPickUp;
	UPROPERTY()
	FOnCreate OnTest;
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
