// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "UI/BOOMUserWidget.h"
#include "BOOMHealthInformation.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMHealthInformation : public UBOOMUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION()
	void SetShieldBar(const float InPercent);
	UFUNCTION()
	void SetHealthBar(const float InPercent);

protected:
	//there is a warning to not use blueprintreadwrite on protected.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* ShieldBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;
};
