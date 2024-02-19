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
    MaxVaultOverDistanceMultiplier = 0.8f;

    MaxMantleHeightDistanceMultiplier = 1.5f;
    MinMantleHeightDistanceMultiplier = 0.85f;

    MaxVaultHeightDistanceMultiplier = MinMantleHeightDistanceMultiplier;
    MinVaultHeightDistanceMultiplier = 0.f;
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
    const float CapsuleHeight = CapsuleHalfHeight * 2;
    const float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
    const FVector CapsuleBaseLocation = Character->GetActorLocation() + FVector::DownVector * CapsuleHalfHeight;

    FVector StartTrace = Character->GetActorLocation() + FVector::UpVector * CapsuleHalfHeight;
    FVector EndTrace = StartTrace + Character->GetActorForwardVector() * CapsuleRadius * MaxHorizontalReachDistanceMultiplier;

    //DETECT SIDE SURFACE
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(Character);
    FHitResult HitResultFront;
    float SideHeightTrace = CapsuleHeight;
    float SideHeightTraceIncrement = SideHeightTrace / NumSurfaceSideTraceQueries;
    bool bHitFront = false;

    //Raycast to find side of a vault/mantle surface.
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

    //TOP SURFACE CHECKS
    TArray<FHitResult> HitResultsTop;

    /*Projects vector onto plane's normal vector, then uses vector subtraction to find vector on plane.*/
    FVector UpVectorProjectedOntoPlaneResult = FVector::VectorPlaneProject(FVector::UpVector, HitResultFront.Normal);
    UpVectorProjectedOntoPlaneResult.Normalize(); //Leaving this vector non-normalized at the moment since it works.
   
    //Trace along slope of the surface hit
    //@TODO: May need to adjust vector length to account for slopes

    FVector StartTraceTop = HitResultFront.Location + UpVectorProjectedOntoPlaneResult * CapsuleHalfHeight;
    StartTraceTop += Character->GetActorRotation().Vector() * 2;
    FVector EndTraceTop = HitResultFront.Location;

    bool bHitTop = GetWorld()->LineTraceMultiByChannel(HitResultsTop, StartTraceTop, EndTraceTop, ECC_Visibility, TraceParams);
    DrawDebugLine(GetWorld(), StartTraceTop, EndTraceTop, FColor::White, true);
    
    if (!bHitTop)
    {
        return false;
    }

    UE_LOG(LogTemp, Warning, TEXT("Hits from LineTraceMulti: %d"), HitResultsTop.Num())

    FHitResult HitResultTop = HitResultsTop[0];

    if (!HitResultTop.IsValidBlockingHit())
    {
        return false;
    }

    float SurfaceHeight = (HitResultTop.Location - CapsuleBaseLocation).Z;
    UE_LOG(LogTemp, Warning, TEXT("Surface Height: %f"), SurfaceHeight)

    float TopDotProduct = FMath::Abs(FVector::DotProduct(HitResultTop.Normal, FVector::UpVector));
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


    
    FVector VaultOverLocation = HitResultTop.Location + FVector::UpVector * (CapsuleHalfHeight + 1);

    const FCollisionShape CapsuleQueryVaultOver = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
    FHitResult HitResultCapsuleQueryVaultOver;
    bool bHitCapsuleQueryVaultOver = GetWorld()->SweepSingleByChannel(HitResultCapsuleQueryVaultOver, VaultOverLocation, VaultOverLocation, FQuat::Identity, ECC_Visibility, CapsuleQueryVaultOver, TraceParams);

    if (bHitCapsuleQueryVaultOver)
    {
        DrawDebugCapsule(GetWorld(), VaultOverLocation, CapsuleHalfHeight, CapsuleRadius, FQuat(Character->GetActorRotation()), FColor::Orange, true);

        return false;
    }
    else
    {
        DrawDebugCapsule(GetWorld(), VaultOverLocation, CapsuleHalfHeight, CapsuleRadius, FQuat(Character->GetActorRotation()), FColor::Green, true);
    }

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

    //TEST IF PLAYER CAN FIT IN AREA
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

    ////BACK TRACE CHECKS - for determining if we have enough length to mantle onto, vault over.
    //FVector BackTraceReference = HitResultFront.Location - (HitResultFront.Normal * MaxVaultOverDistanceMultiplier * CapsuleRadius);
    //FHitResult HitResultBack;
    //bool bHitBack = GetWorld()->LineTraceSingleByChannel(HitResultBack, BackTraceReference, HitResultFront.Location , ECC_Visibility, TraceParams);
    //DrawDebugLine(GetWorld(), BackTraceReference, HitResultFront.Location, FColor::Orange, true);

    //if (!bHitBack)
    //{
    //    return false;
    //}

    //Mantle code

    ////SetMovementMode(MOVE_Flying); //@TODO: Thought this would affect gravity.
    //MantleRootMotionSource = MakeShared<FRootMotionSource_MoveToDynamicForce>();

    //MantleRootMotionSource->StartLocation = Character->GetActorLocation();
    //MantleRootMotionSource->TargetLocation = FVector(HitResultFront.Location.X, HitResultFront.Location.Y,  0) + (HitResultFront.Normal * CapsuleRadius) + FVector::UpVector * GetActorLocation().Z;
    //MantleRootMotionSource->Duration = DebugTimeToLineUpMantle;
    ////testing this out
   
    //ApplyRootMotionSource(MantleRootMotionSource);

    //if (MantleMontage3P)
    //{
    //    //adjust based on movement parameters
    //    Character->PlayAnimMontage(MantleMontage3P, 1.f ); //adjust animation speed based on movement parameters.
    //}
    
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
