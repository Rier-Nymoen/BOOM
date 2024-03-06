// Project BOOM


#include "Character/BOOMCharacterMovementComponent.h"
#include "Character/BOOMCharacter.h"
#include "Components/CapsuleComponent.h"
#include "CollisionShape.h"
#include "Kismet/KismetSystemLibrary.h"

UBOOMCharacterMovementComponent::UBOOMCharacterMovementComponent()
{
    //Might alter these movement values to be percentages of the capsule half height.
    MaxMantleVerticalReach = 225.f;
    DebugTimeToLineUpMantle = 0.2f;

    MaxHorizontalReachDistanceMultiplier = 1.4f; //Might want to extend higher depending on how the "line up phase" of animation ends up feeling.
    MinHorizontalReachDistanceMultiplier = 1.085f;

    MinimumMantleSteepnessAngle = 45.f;
    NumSurfaceSideTraceQueries = 10;
    bIsInMantle = false;

    bWantsToMantle = false;
    bCanMantle = true;

    bWantsToVault = false;
    bCanVault = true;
    MaxVaultOverDistanceMultiplier = 0.8f;

    MaxMantleHeightDistanceMultiplier = 1.5f;
    MinMantleHeightDistanceMultiplier = 0.85f;

    MaxVaultHeightDistanceMultiplier = MinMantleHeightDistanceMultiplier;
    MinVaultHeightDistanceMultiplier = 0.f;
}

void UBOOMCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UBOOMCharacterMovementComponent::ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds)
{
    Super::ControlledCharacterMove(InputVector, DeltaSeconds);
}


void UBOOMCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
    if (bBOOMPressedJump)
    {
        if (CanPerformAlternateJumpMovement())
        {
            bIsInMantle = true;
            CharacterOwner->StopJumping();
        }
    }

    Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UBOOMCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
    Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

    //definitely need to account for more factors than just this.
    if (bIsInMantle && !HasRootMotionSources())
    {
        bIsInMantle = false;

        SetMovementMode(EMovementMode::MOVE_Walking);
    }
   
}

bool UBOOMCharacterMovementComponent::CanPerformAlternateJumpMovement()
{
    if (bIsInMantle)
    {
        return false;
    }

    ACharacter* Character = GetCharacterOwner();

    const float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    const float CapsuleHeight = CapsuleHalfHeight * 2;
    const float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
    const FVector CapsuleBaseLocation = Character->GetActorLocation() + FVector::DownVector * CapsuleHalfHeight;
    
    FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.f);
    
    //velocity relative to actor forward vector
    float ForwardSpeed = FVector::DotProduct(Velocity, Character->GetActorForwardVector());

    float MantleReachDistance = FMath::Clamp(ForwardSpeed, CapsuleRadius* MinHorizontalReachDistanceMultiplier, CapsuleRadius * MaxHorizontalReachDistanceMultiplier);

    UE_LOG(LogTemp, Warning, TEXT("Forward Speed: %f"), ForwardSpeed)
    UE_LOG(LogTemp, Warning, TEXT("MantleReachDistance: %f"), MantleReachDistance)
    FVector StartTrace = Character->GetActorLocation() + FVector::UpVector * CapsuleHalfHeight;
    FVector EndTrace = StartTrace + Character->GetActorForwardVector() * MantleReachDistance;

    //DETECT SIDE SURFACE
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(Character);
    FHitResult HitResultFront;
    float SideHeightTrace = CapsuleHeight;
    float SideHeightTraceIncrement = SideHeightTrace / NumSurfaceSideTraceQueries;
    bool bHitFront = false;

    //Raycast to find side of a vault/mantle surface. Could try shape sweeping as well.
    for (int i = 0; i < NumSurfaceSideTraceQueries; i++)
    {
        bHitFront = GetWorld()->LineTraceSingleByChannel(HitResultFront, StartTrace, EndTrace, ECC_Visibility, TraceParams);
        if (bHitFront)
        {
            DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, true);
            break;
        }

        StartTrace += FVector::DownVector * SideHeightTraceIncrement;
        EndTrace += FVector::DownVector * SideHeightTraceIncrement;
    }
    if (!bHitFront)
    {
        return false;
    }
  
    float FrontDotProduct = FMath::Abs(FVector::DotProduct(HitResultFront.Normal, FVector::UpVector));
    float FrontSteepnessRadians = FMath::Acos(FrontDotProduct);
    float FrontSteepnessAngle = FMath::RadiansToDegrees(FrontSteepnessRadians);

    //UE_LOG(LogTemp, Warning, TEXT("Dot Product %f."), UpDotNormal);
    //UE_LOG(LogTemp, Warning, TEXT("Normal Vector %s."), *HitResultFront.Normal.ToString());
    //UE_LOG(LogTemp, Warning, TEXT("Normal Magnitude %f."), HitResultFront.Normal.Length());
    //UE_LOG(LogTemp, Warning, TEXT("Up Vector Magnitude %f."), FVector::UpVector.Length());
    //UE_LOG(LogTemp, Warning, TEXT("Side Steepness Angle: %f."), FrontSteepnessAngle);

    if ((FMath::Abs(FrontSteepnessAngle)) < MinimumMantleSteepnessAngle) //The side angle might not matter at all.
    {
        //UE_LOG(LogTemp, Warning, TEXT("Mantle Surface Failed Side Steepness Check."))
        return false;
    }

    /*Projects vector onto plane's normal vector, then uses vector subtraction to find vector on plane.*/
    FVector UpVectorProjectedOntoPlaneResult = FVector::VectorPlaneProject(FVector::UpVector, HitResultFront.Normal);
    UpVectorProjectedOntoPlaneResult.Normalize();
   
    //Take the projected vector and trace along the slope to find edges.
    FVector StartTraceTop = HitResultFront.Location + UpVectorProjectedOntoPlaneResult * CapsuleHeight;
    StartTraceTop += Character->GetActorRotation().Vector() * 2;
    FVector EndTraceTop = HitResultFront.Location;

    //TOP SURFACE CHECKS
    TArray<FHitResult> HitResultsTop;

    bool bHitTop = GetWorld()->LineTraceMultiByProfile(HitResultsTop, StartTraceTop, EndTraceTop, "OverlapAll", TraceParams);

    DrawDebugLine(GetWorld(), StartTraceTop, EndTraceTop, FColor::White, true);
    //UE_LOG(LogTemp, Warning, TEXT("Hits from LineTraceMulti: %d"), HitResultsTop.Num())

    FHitResult HitResultTop;
    int LowestSurfaceIndex = HitResultsTop.Num() - 1;
    if (HitResultsTop.IsValidIndex(LowestSurfaceIndex))
    {
       HitResultTop = HitResultsTop[LowestSurfaceIndex];
    }

    if (HitResultTop.bStartPenetrating)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Started  Penetrating"))
        return false;
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("Didnt start penetrating"))
    }
    
       
    //if (!HitResultTop.IsValidBlockingHit())
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("Invalid blocking height"))

    //    return false;
    //}

    float SurfaceHeight = (HitResultTop.Location - CapsuleBaseLocation).Z;
    //UE_LOG(LogTemp, Warning, TEXT("Surface Height: %f"), SurfaceHeight)
    
    //could not bother tracing below a certain height.
    if (SurfaceHeight >= CapsuleHeight * MaxMantleHeightDistanceMultiplier || SurfaceHeight <= CapsuleHalfHeight * MinMantleHeightDistanceMultiplier)
    {
        return false;
    }

    float TopDotProduct = FMath::Abs(FVector::DotProduct(HitResultTop.Normal, FVector::UpVector));
    float TopSteepnessRadians = FMath::Acos(TopDotProduct);
    float TopSteepnessAngle = FMath::RadiansToDegrees(TopSteepnessRadians);
    float TopSinAngle;

    //UE_LOG(LogTemp, Warning, TEXT("Top Steepness Angle: %f."), TopSteepnessAngle)

    if ((FMath::Abs(TopSteepnessAngle)) > MinimumMantleSteepnessAngle) //The side angle might not matter at all.
    {
        return false;
    }

    if(!TopSteepnessAngle) // we might not have an angle between
    {
        TopSinAngle = 0.f;
    }
    else
    {
        TopSinAngle =  PI - TopDotProduct - PI/2;
    } 
          
    //
    //FVector VaultOverLocation = HitResultTop.Location + FVector::UpVector * (CapsuleHalfHeight + 1);

    //const FCollisionShape CapsuleQueryVaultOver = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
    //FHitResult HitResultCapsuleQueryVaultOver;
    //bool bHitCapsuleQueryVaultOver = GetWorld()->SweepSingleByChannel(HitResultCapsuleQueryVaultOver, VaultOverLocation, VaultOverLocation, FQuat::Identity, ECC_Visibility, CapsuleQueryVaultOver, TraceParams);

    //if (bHitCapsuleQueryVaultOver)
    //{
    //    DrawDebugCapsule(GetWorld(), VaultOverLocation, CapsuleHalfHeight, CapsuleRadius, FQuat(Character->GetActorRotation()), FColor::Orange, true);

    //    return false;
    //}
    //else
    //{
    //    DrawDebugCapsule(GetWorld(), VaultOverLocation, CapsuleHalfHeight, CapsuleRadius, FQuat(Character->GetActorRotation()), FColor::Green, true);
    //}

    FVector CharacterMantleEndLocation = HitResultTop.Location + Character->GetActorForwardVector() * CapsuleRadius + FVector::UpVector * (CapsuleHalfHeight + 1);

    //@TODO: Not perfect. Can improve later with different approach
    FVector ZOffset = FMath::Sin(TopSinAngle) * FVector::UpVector * HitResultTop.Normal.Z * CapsuleHalfHeight;
    FVector XYOffset = FMath::Sin(TopSinAngle) * FVector(HitResultTop.Normal.X, HitResultTop.Normal.Y, 0) * CapsuleRadius;
    CharacterMantleEndLocation += (ZOffset + XYOffset);

    //UE_LOG(LogTemp, Warning, TEXT("Sin(TopSinAngle): %f"), TopSinAngle);
    //UE_LOG(LogTemp, Warning, TEXT("Cos(TopSinAngle): %f"), TopSteepnessAngle);
    //UE_LOG(LogTemp, Warning, TEXT("TopNormal %s"), *HitResultTopClosest.Normal.ToString());
    //UE_LOG(LogTemp, Warning, TEXT("ZOffset: %s"), *ZOffset.ToString());
    //UE_LOG(LogTemp, Warning, TEXT("XYOffset: %s"), *XYOffset.ToString());
    
    //Player Capsule Clearance Check - determines if player can fit on the climb target.
    const FCollisionShape CapsuleQueryMantleEnd = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
    FHitResult HitResultCapsuleQueryMantleEnd;
    bool bHitCapsuleQueryMantleEnd = GetWorld()->SweepSingleByChannel(HitResultCapsuleQueryMantleEnd, CharacterMantleEndLocation, CharacterMantleEndLocation, FQuat::Identity, ECC_Visibility, CapsuleQueryMantleEnd, TraceParams);

    if (bHitCapsuleQueryMantleEnd)
    {
        DrawDebugCapsule(GetWorld(), CharacterMantleEndLocation, CapsuleHalfHeight, CapsuleRadius, FQuat(Character->GetActorRotation()), FColor::Red, true);
        return false;
    }
    else
    {
        DrawDebugCapsule(GetWorld(), CharacterMantleEndLocation, CapsuleHalfHeight, CapsuleRadius, FQuat(Character->GetActorRotation()), FColor::Cyan, true);
    }

    ////Back Trace Check - for determining if we have enough length to mantle onto, vault over.
    //FVector BackTraceReference = HitResultFront.Location - (HitResultFront.Normal * MaxVaultOverDistanceMultiplier * CapsuleRadius);
    //FHitResult HitResultBack;
    //bool bHitBack = GetWorld()->LineTraceSingleByChannel(HitResultBack, BackTraceReference, HitResultFront.Location , ECC_Visibility, TraceParams);
    //DrawDebugLine(GetWorld(), BackTraceReference, HitResultFront.Location, FColor::Orange, true);

    //if (!bHitBack)
    //{
    //    return false;
    //}

    /*Placeholder for animation root motion to be implemented.*/
    //@TODO: Implement mantling animations with root motion & get rid of magic numbers.

    MantleLineUp_RootMotionSource = MakeShared<FRootMotionSource_MoveToDynamicForce>();
    MantleLineUp_RootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Override;
    MantleLineUp_RootMotionSource->StartLocation = Character->GetActorLocation();
    MantleLineUp_RootMotionSource->TargetLocation = FVector(HitResultFront.Location.X, HitResultFront.Location.Y, GetActorLocation().Z) + (HitResultFront.Normal * CapsuleRadius );
    float TargetDistance = FVector::Distance(MantleLineUp_RootMotionSource->StartLocation, MantleLineUp_RootMotionSource->TargetLocation);
    MantleLineUp_RootMotionSource->Duration = 0.1f;

    ApplyRootMotionSource(MantleLineUp_RootMotionSource);

    MantleClimbUp_RootMotionSource = MakeShared<FRootMotionSource_MoveToDynamicForce>();
    MantleClimbUp_RootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Override;
    MantleClimbUp_RootMotionSource->StartLocation = MantleLineUp_RootMotionSource->TargetLocation;
    MantleClimbUp_RootMotionSource->TargetLocation =  FVector(MantleLineUp_RootMotionSource->TargetLocation.X, MantleLineUp_RootMotionSource->TargetLocation.Y, HitResultTop.Location.Z) + FVector::UpVector * (CapsuleHalfHeight + 1);
    TargetDistance = FVector::Distance(MantleClimbUp_RootMotionSource->StartLocation, MantleClimbUp_RootMotionSource->TargetLocation);
    MantleClimbUp_RootMotionSource->Duration = 0.3f;

    ApplyRootMotionSource(MantleClimbUp_RootMotionSource);

    MantleClimbForward_RootMotionSource = MakeShared<FRootMotionSource_MoveToDynamicForce>();
    MantleClimbForward_RootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Override;
    MantleClimbForward_RootMotionSource->StartLocation = MantleClimbUp_RootMotionSource->TargetLocation;
    MantleClimbForward_RootMotionSource->TargetLocation = CharacterMantleEndLocation;
    MantleClimbForward_RootMotionSource->Duration = 0.6f;

    ApplyRootMotionSource(MantleClimbForward_RootMotionSource);
    return true;
}

bool UBOOMCharacterMovementComponent::StartMantle()
{

    /*Right now jumping works by ACharacter calling DoJump.*/
    return CanPerformAlternateJumpMovement();
}

/*Evaluates which jump action to take based on character and wall height.*/
void UBOOMCharacterMovementComponent::DetermineJumpInputAction()
{
    /*
    - Trace to get the wall height.
    - Depending on the percent of the wall height relative to the player height
    - We can either vault or mantle. At various different heights.
    */


}

bool UBOOMCharacterMovementComponent::CanMantle()
{
    return true;
}


void UBOOMCharacterMovementComponent::PerformMantle()
{
}
