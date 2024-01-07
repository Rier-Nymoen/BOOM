// Project BOOM

#pragma once

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#include "CoreMinimal.h"
#include "AttributeSet.h"

#include "AbilitySystemComponent.h"

#include "BOOMAttributeSetBase.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
public:
	UBOOMAttributeSetBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;


	UPROPERTY(BlueprintReadOnly, Category = "Health"/*, ReplicatedUsing = OnRep_Health*/)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBOOMAttributeSetBase, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Health"/*, ReplicatedUsing = OnRep_MaxHealth*/)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBOOMAttributeSetBase, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData ShieldStrength;
	ATTRIBUTE_ACCESSORS(UBOOMAttributeSetBase, ShieldStrength)

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxShieldStrength;
	ATTRIBUTE_ACCESSORS(UBOOMAttributeSetBase, MaxShieldStrength)


	//Meta Attributes
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UBOOMAttributeSetBase, Damage)


	//UFUNCTION()
	//virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
};
