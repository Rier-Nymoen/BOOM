// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BOOMHealthComponent.h"
#include "AbilitySystemComponent.h"
#include "Character/BOOMAttributeSetBase.h"
#include "AIController.h"

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

	AttributeSetBase = AbilitySystemComponent->GetSet<UBOOMAttributeSetBase>();
	if (AttributeSetBase == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute Set is nulltpr"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute Set is not null"))

	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &UBOOMHealthComponent::HandleHealthChanged);
}

void UBOOMHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Broadcasting OnHealthChanged inside UBOOMHealthComponent::HandleHealthChanged()."), *GetName());
	OnHealthChanged.Broadcast(Data.OldValue, Data.NewValue);
}

float UBOOMHealthComponent::GetMaxHealth() const
{
	return AttributeSetBase ? AttributeSetBase->GetMaxHealth() : 0.0f;
}

float UBOOMHealthComponent::GetHealth() const
{
	return AttributeSetBase ? AttributeSetBase->GetHealth() : 0.0f;
}

float UBOOMHealthComponent::GetHealthPercentage() const
{
	if (AttributeSetBase)
	{
		float MaxHealth = AttributeSetBase->GetMaxHealth();
		return MaxHealth > 0.f ? AttributeSetBase->GetHealth() / MaxHealth : 0.0f;
	}
	return 0.0f;
}





