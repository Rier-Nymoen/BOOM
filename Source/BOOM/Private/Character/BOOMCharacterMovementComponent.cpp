// Project BOOM


#include "Character/BOOMCharacterMovementComponent.h"
#include "Character/BOOMCharacter.h"
#include "Components/CapsuleComponent.h"
#include "CollisionShape.h"

UBOOMCharacterMovementComponent::UBOOMCharacterMovementComponent()
{
    MaxMantleHorizontalReach = 255.f;
    MaxMantleVerticalReach = 10.f;
    MinimumMantleSteepnessAngle = 77.f;
    NumMantleTraceQueries = 10;
    MantleCapsuleQueryHeightOffset = 11.f;
}

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
    float TraceHeightIncrement = (CapsuleHalfHeight * 2) / NumMantleTraceQueries;
    bool bHitFront;

    //Conduct Raycasts to find side of a mantleable surface.
    for (int i = 0; i < NumMantleTraceQueries; i++)
    {
        StartTrace += FVector::DownVector * TraceHeightIncrement;
        EndTrace = StartTrace + (Character->GetActorForwardVector() * MaxMantleHorizontalReach);

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

    //@TODO: Add code to detect the steepness of the surface to see if it is climbable. Take dot product of normal up vector, then get the angle to determine.
        
    float FrontProjectionRatio = FMath::Abs(FVector::DotProduct(HitResultFront.Normal, FVector::UpVector));
    float FrontSteepnessRadians = FMath::Acos(FrontProjectionRatio);
    float FrontSteepnessAngle = FMath::RadiansToDegrees(FrontSteepnessRadians);

    //UE_LOG(LogTemp, Warning, TEXT("Dot Product %f."), UpDotNormal);
    //UE_LOG(LogTemp, Warning, TEXT("Normal Vector %s."), *HitResultFront.Normal.ToString());
    //UE_LOG(LogTemp, Warning, TEXT("Normal Magnitude %f."), HitResultFront.Normal.Length());
    //UE_LOG(LogTemp, Warning, TEXT("Up Vector Magnitude %f."), FVector::UpVector.Length());
    //UE_LOG(LogTemp, Warning, TEXT("Side Steepness Angle: %f."), FrontSteepnessAngle);

    if (( FMath::Abs(FrontSteepnessAngle)) < MinimumMantleSteepnessAngle) //The side angle might not matter at all. It could be the angle of what you're going to stand upon. 
    {
        //UE_LOG(LogTemp, Warning, TEXT("Mantle Surface Failed Side Steepness Check."))
        return false;
    }

    //BACK TRACE CHECKS
    //@TODO: Calculations of jump onto or over.
    FVector BackTraceReference = CapsuleRadius * Character->GetActorForwardVector() + HitResultFront.Location;

    FHitResult HitResultBack;
    bool bHitBack = GetWorld()->LineTraceSingleByChannel(HitResultBack, HitResultFront.Location, BackTraceReference, ECC_Visibility, TraceParams);
    //DrawDebugLine(GetWorld(), HitResultFront.Location, BackTraceReference, FColor::Emerald, true);

    if (!bHitBack)
    {
        return false;
    }

    //TOP SURFACE CHECKS
   
    //Finds top of climbable surface (the part we stand on) with vector projections!
    TArray<FHitResult> HitResultsTop;

    /*Projects vector onto plane's normal vector, then uses vector subtraction to find vector on plane.*/
    FVector UpVectorProjectedOntoPlaneResult = FVector::VectorPlaneProject(FVector::UpVector, HitResultFront.Normal);

    //Trace along slope of the surface hit
    //@TODO: May need to adjust vector length to account for slopes
    FVector StartTraceTop = (UpVectorProjectedOntoPlaneResult * 2 * CapsuleHalfHeight) + HitResultFront.Location;

    //Need to move forward by some amount because the trace can barely hit or miss the top of the surface we want to climb on.
    StartTraceTop += Character->GetActorRotation().Vector() * 2;

    FVector EndTraceTop = StartTraceTop - (UpVectorProjectedOntoPlaneResult * CapsuleHalfHeight * 2);

    bool bHitTop = GetWorld()->LineTraceMultiByChannel(HitResultsTop, StartTraceTop, EndTraceTop, ECC_Visibility, TraceParams);
    DrawDebugLine(GetWorld(), StartTraceTop, EndTraceTop, FColor::White, true);

    if (!bHitTop)
    {
        return false;
    }
    //UE_LOG(LogTemp, Warning, TEXT("Hits from LineTraceMulti: %d"), HitResultsTop.Num())

    //@TODO: we aren't receiving multiple hits when we should. Why?

    FHitResult HitResultTopClosest = HitResultsTop[HitResultsTop.Num()-1];

    float TopProjectionRatio = FMath::Abs(FVector::DotProduct(HitResultTopClosest.Normal, FVector::UpVector));
    float TopSteepnessRadians = FMath::Acos(TopProjectionRatio);
    float TopSteepnessAngle = FMath::RadiansToDegrees(TopSteepnessRadians);
    float TopSinAngle = 180 - 90 - TopSteepnessAngle; //we might not have an angle between up and normal. Handle it!

    FVector ActorMantleCenterLocation = HitResultTopClosest.Location + Character->GetActorForwardVector() * CapsuleRadius + FVector::UpVector * CapsuleHalfHeight;


    FVector ZOffset = FMath::Sin(TopSinAngle) * FVector::UpVector * HitResultTopClosest.Normal.Z * CapsuleHalfHeight;
    FVector XYOffset = FMath::Sin(TopSinAngle) * FVector(HitResultTopClosest.Normal.X, HitResultTopClosest.Normal.Y, 0) * CapsuleRadius;
    UE_LOG(LogTemp, Warning, TEXT("Sin(TopSinAngle): %f"), TopSinAngle);
    UE_LOG(LogTemp, Warning, TEXT("Cos(TopSinAngle): %f"), TopSteepnessAngle);
    UE_LOG(LogTemp, Warning, TEXT("TopNormal %s"), *HitResultTopClosest.Normal.ToString());

    UE_LOG(LogTemp, Warning, TEXT("ZOffset: %s"), *ZOffset.ToString());
    UE_LOG(LogTemp, Warning, TEXT("XYOffset: %s"), *XYOffset.ToString());


    ActorMantleCenterLocation += ZOffset + XYOffset;
        
    //ActorMantleCenterLocation += FVector::UpVector * MantleCapsuleQueryHeightOffset; //temporary fix

    //ActorMantleCenterLocation += 

    /*
    @TODO: Ramps can cause the capsule check to overlap part of the slope, giving incorrect results. A small Z-axis Offset is a "jank" fix.
    Try to relate the capsule query settings to the angle and capsule radius.
    */

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
        return true;
    }

    if (MantleMontage3P)
    {
        Character->PlayAnimMontage(MantleMontage3P, 1.F); //adjust animation speed based on movement parameters.
    }
        
    return false;

    /*If we are on a top surface that has a normal perfectly up.

We just account for the capsule height and a few minor offset to properly position the characters every time.

As the angle of the slope increases,*/


}

void UBOOMCharacterMovementComponent::StartMantle()
{
    if (DetectMantleableSurface())
    {
        PerformMantle(); //could do something like FMantleContext OutMantleContext for all necessary information to do the actual movement and animation control.
    }

}

void UBOOMCharacterMovementComponent::PerformMantle()
{
}
