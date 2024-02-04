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


	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite)
	int NumMantleTraceQueries;


	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite)
	int MantleCapsuleQueryHeightOffset;

	UPROPERTY(Category = "Character Movement: Mantle", EditDefaultsOnly)
	UAnimMontage* MantleMontage3P;
	UPROPERTY(Category = "Character Movement: Mantle", EditDefaultsOnly)
	UAnimMontage* MantleMontage1P;

	TSharedPtr<FRootMotionSource_MoveToDynamicForce> MantleRootMotionSource;


	/*move to protected after testing*/

	virtual void StartMantle();

protected:

	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds);

	virtual bool DetectMantleableSurface();

	virtual void PerformMantle();

	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds) override;
	/*Code for climbing/mantling and stepping up on surfaces.*/

	bool bIsInMantle;

	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere)
	float DebugTimeToLineUpMantle;



};
