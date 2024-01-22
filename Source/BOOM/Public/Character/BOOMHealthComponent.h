// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"

#include "BOOMHealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBOOMOnHealthChanged, float, OldValue, float, NewValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOOM_API UBOOMHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBOOMHealthComponent();


	UPROPERTY()
	FBOOMOnHealthChanged OnHealthChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY()
	class UAbilitySystemComponent* AbilitySystemComponent;
	//figure out health and shield interactions
	UPROPERTY()
	const class UBOOMAttributeSetBase* AttributeSetBase;
	

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitializeComponentWithOwningActor(class UAbilitySystemComponent* InAbilitySystemComponent);

	//UFUNCTION()
	void HandleHealthChanged(const FOnAttributeChangeData& Data);

private:
		
};
