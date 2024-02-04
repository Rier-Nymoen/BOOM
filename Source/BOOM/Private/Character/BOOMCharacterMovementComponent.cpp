// Project BOOM


#include "Character/BOOMCharacterMovementComponent.h"
#include "Character/BOOMCharacter.h"
#include "Components/CapsuleComponent.h"
#include "CollisionShape.h"

UBOOMCharacterMovementComponent::UBOOMCharacterMovementComponent()
{
    MaxMantleHorizontalReach = 225.f;
    MaxMantleVerticalReach = 10.f;
    MinimumMantleSteepnessAngle = 77.f;
    NumMantleTraceQueries = 10;
    MantleCapsuleQueryHeightOffset = 11.f;
    bIsInMantle = false;
    DebugTimeToLineUpMantle = 0.2f;

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
    bool bHitFront = false;

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
    float TopSinAngle;
    if(!TopSteepnessAngle) // we might not have an angle between
    {
        TopSinAngle = 0;
    }
    else
    {
        TopSinAngle = 180 - 90 - TopSteepnessAngle;
    }

    FVector ActorMantleCenterLocation = HitResultTopClosest.Location + Character->GetActorForwardVector() * CapsuleRadius + FVector::UpVector * CapsuleHalfHeight;


    //@TODO: Not perfect. Can improve later with different approach
    FVector ZOffset = FMath::Sin(TopSinAngle) * FVector::UpVector * HitResultTopClosest.Normal.Z * CapsuleHalfHeight;
    FVector XYOffset = FMath::Sin(TopSinAngle) * FVector(HitResultTopClosest.Normal.X, HitResultTopClosest.Normal.Y, 0) * CapsuleRadius;
    UE_LOG(LogTemp, Warning, TEXT("Sin(TopSinAngle): %f"), TopSinAngle);
    UE_LOG(LogTemp, Warning, TEXT("Cos(TopSinAngle): %f"), TopSteepnessAngle);
    UE_LOG(LogTemp, Warning, TEXT("TopNormal %s"), *HitResultTopClosest.Normal.ToString());

    UE_LOG(LogTemp, Warning, TEXT("ZOffset: %s"), *ZOffset.ToString());
    UE_LOG(LogTemp, Warning, TEXT("XYOffset: %s"), *XYOffset.ToString());


    ActorMantleCenterLocation += (ZOffset + XYOffset);

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
    

    bIsInMantle = true;
    return true;
}

void UBOOMCharacterMovementComponent::StartMantle()
{
    if (DetectMantleableSurface())
    {
        //if i want to separate the functions for seeing if the character is able to mantle, and the logic for actually moving into the mantle
        // could do something like FMantleContext OutMantleContext for all necessary information to do the actual movement and animation control.
        PerformMantle(); 

    }

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
