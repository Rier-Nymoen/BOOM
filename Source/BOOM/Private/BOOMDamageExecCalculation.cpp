// Project BOOM


#include "BOOMDamageExecCalculation.h"

#include "BOOMAttributeSetBase.h"


struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health)
	//DECLARE_ATTRIBUTE_CAPTUREDEF(Damage)

	FDamageStatics()
	{
		//Careful incase called on self causes bug
		//DEFINE_ATTRIBUTE_CAPTUREDEF(UBOOMAttributeSetBase, Health, Target, false);

	}

	//FGameplayEffectAttributeCaptureDefinition
};

UBOOMDamageExecCalculation::UBOOMDamageExecCalculation()
{
}

void UBOOMDamageExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UE_LOG(LogTemp, Warning, TEXT("DamageExecCalculationCalled"))
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	/*
	* TODO: 
	*/
	
}
