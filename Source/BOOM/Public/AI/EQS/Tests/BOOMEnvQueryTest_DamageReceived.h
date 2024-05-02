// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "BOOMEnvQueryTest_DamageReceived.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMEnvQueryTest_DamageReceived : public UEnvQueryTest
{
	GENERATED_BODY()
	
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;

};
