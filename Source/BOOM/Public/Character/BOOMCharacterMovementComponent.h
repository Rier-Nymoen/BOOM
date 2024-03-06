// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BOOMCharacterMovementComponent.generated.h"

class ABOOMCharacter;

struct FJumpActionSurfaceData
{
	FHitResult HitResultFront;
	FHitResult HitResultTop;
	FVector UpVectorProjectedOntoPlaneResult;


};

UCLASS()
class BOOM_API UBOOMCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	
	UBOOMCharacterMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm"))
	float MaxMantleVerticalReach;
	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm"))
	float MaxMantleHorizontalReach;



	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "degrees"))
	float MinimumMantleSteepnessAngle;

	bool bBOOMPressedJump;
	
	ABOOMCharacter* BOOMCharacterOwner;


	UPROPERTY(Category = "Character Movement: Mantle", EditAnywhere, BlueprintReadWrite)
	int NumSurfaceSideTraceQueries;

	UPROPERTY(Category = "Character Movement: Mantle", EditDefaultsOnly)
	UAnimMontage* MantleMontage3P;
	UPROPERTY(Category = "Character Movement: Mantle", EditDefaultsOnly)
	UAnimMontage* MantleMontage1P;

	TSharedPtr<FRootMotionSource_MoveToDynamicForce> MantleLineUp_RootMotionSource;
	TSharedPtr<FRootMotionSource_MoveToDynamicForce> MantleClimbUp_RootMotionSource;
	TSharedPtr<FRootMotionSource_MoveToDynamicForce> MantleClimbForward_RootMotionSource;


	/*The multiplier for the character capsule radius when determining how far the character can reach.*/
	UPROPERTY(EditAnywhere)
	float MaxHorizontalReachDistanceMultiplier;

	UPROPERTY(EditAnywhere)
	float MinHorizontalReachDistanceMultiplier;

	/*The multiplier for the character capsule radius when determining how far the character can vault over obstacles.*/
	UPROPERTY(EditAnywhere)
	float MaxVaultOverDistanceMultiplier;

	UPROPERTY(EditAnywhere)
	float MaxVaultHeightDistanceMultiplier;

	UPROPERTY(EditAnywhere)
	float MinVaultHeightDistanceMultiplier;

	UPROPERTY(EditAnywhere)
	float MaxMantleHeightDistanceMultiplier;

	UPROPERTY(EditAnywhere)
	float MinMantleHeightDistanceMultiplier;


	/*move to protected after testing*/

	virtual bool StartMantle();
	
	virtual void DetermineJumpInputAction();

	virtual bool CanMantle();

	//virtual void QueryMovementSurfaceContext();

protected:

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	virtual bool CanPerformAlternateJumpMovement();

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
