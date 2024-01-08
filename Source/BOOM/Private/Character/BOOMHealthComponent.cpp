// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BOOMHealthComponent.h"
#include "AbilitySystemComponent.h"
#include "Character/BOOMAttributeSetBase.h"

// Sets default values for this component's properties
UBOOMHealthComponent::UBOOMHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	AbilitySystemComponent = nullptr;
	AttributeSetBase = nullptr;

}


void UBOOMHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
	}
	
}


// Called every frame
void UBOOMHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBOOMHealthComponent::InitializeComponentWithOwningActor(UAbilitySystemComponent* InAbilitySystemComponent)
{
	AActor* Owner = GetOwner();


	check(Owner)
	
	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s]: already is already initialized with an AbilitySystemComponent with [%s]."), *GetName(), *Owner->GetName());
	}
	AbilitySystemComponent = InAbilitySystemComponent;
	UE_LOG(LogTemp, Warning, TEXT("[%s]: name test UELOG[%s]."), *GetName(), *Owner->GetName());

	AttributeSetBase = AbilitySystemComponent->GetSet<UBOOMAttributeSetBase>();

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &UBOOMHealthComponent::HandleHealthChanged);

}

void UBOOMHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{

}



