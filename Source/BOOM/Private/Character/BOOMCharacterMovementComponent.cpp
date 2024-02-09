// Project BOOM


#include "Character/BOOMCharacterMovementComponent.h"
#include "Character/BOOMCharacter.h"
#include "Components/CapsuleComponent.h"
#include "CollisionShape.h"

UBOOMCharacterMovementComponent::UBOOMCharacterMovementComponent()
{
    //Might alter these movement values to be percentages of the capsule half height.
    MaxMantleVerticalReach = 225.f;
    DebugTimeToLineUpMantle = 0.2f;

    MaxHorizontalReachDistanceMultiplier = 2.5f;

    MinimumMantleSteepnessAngle = 45.f;
    NumSurfaceSideTraceQueries = 10;
    bIsInMantle = false;

    bWantsToMantle = false;
    bCanMantle = true;

    bWantsToVault = false;
    bCanVault = true;
    MaxVaultOverDistanceMultiplier = 0.5f;

    MaxMantleHeightDistanceMultiplier = 1.5f;
}

//void UBOOMCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//}

void UBOOMCharacterMovementComponent::ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds)
{
    Super::ControlledCharacterMove(InputVector, DeltaSeconds);
}


bool UBOOMCharacterMovementComponent::DetectMantleableSurface()
{
    ACharacter* Character = GetCharacterOwner();

    const float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    const float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

    FVector StartTrace = Character->GetActorLocation() - FVector::DownVector * CapsuleHalfHeight;
    FVector EndTrace;

    //DETECT SIDE SURFACE
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(Character);
    FHitResult HitResultFront;
    float SideHeightTrace = CapsuleHalfHeight * 2; 
    float SideHeightTraceIncrement = SideHeightTrace / NumSurfaceSideTraceQueries;
    bool bHitFront = false;

    //Raycast to find side of a vault/mantle surface.
    for (int i = 0; i < NumSurfaceSideTraceQueries; i++)
    {
        StartTrace += FVector::DownVector * SideHeightTraceIncrement;
        EndTrace = StartTrace + (Character->GetActorForwardVector() * CapsuleRadius * MaxHorizontalReachDistanceMultiplier);

        bHitFront = GetWorld()->LineTraceSingleByChannel(HitResultFront, StartTrace, EndTrace, ECC_Visibility, TraceParams);

        if (!bHitFront)
        {
            //DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true);
            continue;
        }
        else
        {
            DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, true);
            break;
        }
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

    if ((FMath::Abs(FrontSteepnessAngle)) < MinimumMantleSteepnessAngle) //The side angle might not matter at all. It could be the angle of what you're going to stand upon. 
    {
        //UE_LOG(LogTemp, Warning, TEXT("Mantle Surface Failed Side Steepness Check."))
        return false;
    }

    //TOP SURFACE CHECKS
    TArray<FHitResult> HitResultsTop;

    /*Projects vector onto plane's normal vector, then uses vector subtraction to find vector on plane.*/
    FVector UpVectorProjectedOntoPlaneResult = FVector::VectorPlaneProject(FVector::UpVector, HitResultFront.Normal);

    //Trace along slope of the surface hit
    //@TODO: May need to adjust vector length to account for slopes
    FVector StartTraceTop = (UpVectorProjectedOntoPlaneResult * CapsuleHalfHeight * 2 * MaxMantleHeightDistanceMultiplier ) + HitResultFront.Location;

    //Need to move forward by some amount because the trace can barely hit or miss the top of the surface we want to climb on.
    StartTraceTop += Character->GetActorRotation().Vector() * 2;

    FVector EndTraceTop = StartTraceTop - (UpVectorProjectedOntoPlaneResult * CapsuleHalfHeight * 2 * MaxMantleHeightDistanceMultiplier);

    //bool bHitTop = GetWorld()->LineTraceMultiByChannel(HitResultsTop, StartTraceTop, EndTraceTop, ECC_Visibility, TraceParams);
    bool bHitTop = GetWorld()->LineTraceMultiByChannel(HitResultsTop, StartTraceTop, EndTraceTop, ECC_Visibility, TraceParams);
    DrawDebugLine(GetWorld(), StartTraceTop, EndTraceTop, FColor::White, true);
    
    if (!bHitTop)
    {
        return false;
    }
    UE_LOG(LogTemp, Warning, TEXT("Hits from LineTraceMulti: %d"), HitResultsTop.Num())
    //@TODO: we aren't receiving multiple hits when we should. Why?
    FHitResult HitResultTopClosest = HitResultsTop[HitResultsTop.Num()-1];

    if (!HitResultTopClosest.IsValidBlockingHit())
    {
        return false;
    }

    float TopDotProduct = FMath::Abs(FVector::DotProduct(HitResultTopClosest.Normal, FVector::UpVector));
    float TopSteepnessRadians = FMath::Acos(TopDotProduct);
    float TopSteepnessAngle = FMath::RadiansToDegrees(TopSteepnessRadians);
    float TopSinAngle;

    if(!TopSteepnessAngle) // we might not have an angle between - careful of float errors
    {
        TopSinAngle = 0.f;
    }
    else
    {
        TopSinAngle =  PI - TopDotProduct - PI/2;
    }


    //BACK TRACE CHECKS - for determining if we have enough length to mantle onto, vault over.
    //@TODO: Calculations of jump onto or over (mantle or vault).
    FVector BackTraceReference = CapsuleRadius * Character->GetActorForwardVector() + HitResultFront.Location;
    FHitResult HitResultBack;
    bool bHitBack = GetWorld()->LineTraceSingleByChannel(HitResultBack, HitResultFront.Location, BackTraceReference, ECC_Visibility, TraceParams);
    DrawDebugLine(GetWorld(), HitResultFront.Location, BackTraceReference, FColor::Orange, true);

    if (!bHitBack)
    {
        return false;
    }

    FVector ActorMantleCenterLocation = HitResultTopClosest.Location + Character->GetActorForwardVector() * CapsuleRadius + FVector::UpVector * CapsuleHalfHeight;

    //@TODO: Not perfect. Can improve later with different approach
    FVector ZOffset = FMath::Sin(TopSinAngle) * FVector::UpVector * HitResultTopClosest.Normal.Z * CapsuleHalfHeight;
    FVector XYOffset = FMath::Sin(TopSinAngle) * FVector(HitResultTopClosest.Normal.X, HitResultTopClosest.Normal.Y, 0) * CapsuleRadius;
    ActorMantleCenterLocation += (ZOffset + XYOffset);

    UE_LOG(LogTemp, Warning, TEXT("Sin(TopSinAngle): %f"), TopSinAngle);
    UE_LOG(LogTemp, Warning, TEXT("Cos(TopSinAngle): %f"), TopSteepnessAngle);
    UE_LOG(LogTemp, Warning, TEXT("TopNormal %s"), *HitResultTopClosest.Normal.ToString());
    UE_LOG(LogTemp, Warning, TEXT("ZOffset: %s"), *ZOffset.ToString());
    UE_LOG(LogTemp, Warning, TEXT("XYOffset: %s"), *XYOffset.ToString());


    //TEST IF PLAYER CAN FIT IN AREA
    const FCollisionShape CapsuleQuery = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
    FHitResult HitResultCapsuleQuery;
    bool bHitCapsuleQuery = GetWorld()->SweepSingleByChannel(HitResultCapsuleQuery, ActorMantleCenterLocation, ActorMantleCenterLocation, FQuat::Identity, ECC_Visibility, CapsuleQuery);

    if (bHitCapsuleQuery)
    {
        DrawDebugCapsule(GetWorld(), ActorMantleCenterLocation, CapsuleHalfHeight, CapsuleRadius, FQuat(Character->GetActorRotation()), FColor::Red, true);

        return false;
    }
    else
    {
        DrawDebugCapsule(GetWorld(), ActorMantleCenterLocation, CapsuleHalfHeight, CapsuleRadius, FQuat(Character->GetActorRotation()), FColor::Cyan, true);
    }

    //SetMovementMode(MOVE_Flying); //@TODO: Thought this would affect gravity.
    MantleRootMotionSource = MakeShared<FRootMotionSource_MoveToDynamicForce>();

    MantleRootMotionSource->StartLocation = Character->GetActorLocation();
    MantleRootMotionSource->TargetLocation = FVector(HitResultFront.Location.X, HitResultFront.Location.Y,  0) + (HitResultFront.Normal * CapsuleRadius) + FVector::UpVector * GetActorLocation().Z;
    MantleRootMotionSource->Duration = DebugTimeToLineUpMantle;
    //testing this out
   
    ApplyRootMotionSource(MantleRootMotionSource);

    if (MantleMontage3P)
    {
        //adjust based on movement parameters
        Character->PlayAnimMontage(MantleMontage3P, 1.f ); //adjust animation speed based on movement parameters.
    }
    
    return true;
}

void UBOOMCharacterMovementComponent::StartMantle()
{

    /*Right now jumping works by ACharacter calling DoJump.*/
    DetectMantleableSurface();
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

void UBOOMCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
    Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

    //definitely need to account for more factors than just this.
    if (!HasRootMotionSources() && MOVE_Flying == MovementMode && bIsInMantle)
    {
        SetMovementMode(MOVE_Walking);
    }
}
