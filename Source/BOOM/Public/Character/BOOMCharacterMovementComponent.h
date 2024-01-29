// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BOOMCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	
	UBOOMCharacterMovementComponent();

	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm"))
	float MaxMantleVerticalReach;
	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm"))
	float MaxMantleHorizontalReach;

	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "degrees"))
	float MinimumMantleSteepnessAngle;




	/*move to protected after testing*/
	virtual bool DetectMantleableSurface();

protected:

	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds) override;
	/*Code for climbing/mantling and stepping up on surfaces.*/




};
