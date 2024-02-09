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

	//virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm"))
	float MaxMantleVerticalReach;
	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm"))
	float MaxMantleHorizontalReach;

	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "degrees"))
	float MinimumMantleSteepnessAngle;


	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite)
	int NumSurfaceSideTraceQueries;

	UPROPERTY(Category = "Character Movement: Mantle", EditDefaultsOnly)
	UAnimMontage* MantleMontage3P;
	UPROPERTY(Category = "Character Movement: Mantle", EditDefaultsOnly)
	UAnimMontage* MantleMontage1P;

	TSharedPtr<FRootMotionSource_MoveToDynamicForce> MantleRootMotionSource;

	/*The multiplier for the character capsule radius when determining how far the character can reach.*/
	UPROPERTY(EditAnywhere)
	float MaxHorizontalReachDistanceMultiplier;

	/*The multiplier for the character capsule radius when determining how far the character can vault over obstacles.*/
	UPROPERTY(EditAnywhere)
	float MaxVaultOverDistanceMultiplier;

	UPROPERTY(EditAnywhere)
	float MaxMantleHeightDistanceMultiplier;


	/*move to protected after testing*/

	virtual void StartMantle();
	
	virtual void DetermineJumpInputAction();

	virtual bool CanMantle();

protected:

	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	virtual bool DetectMantleableSurface();

	virtual void PerformMantle();

	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds) override;
	/*Code for climbing/mantling and stepping up on surfaces.*/
	
	bool bWantsToMantle;
	bool bWantsToVault;

	UPROPERTY(EditAnywhere)
	bool bCanMantle;
	UPROPERTY(EditAnywhere)
	bool bCanVault;

	bool bIsInMantle;

	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere)
	float DebugTimeToLineUpMantle;



};
