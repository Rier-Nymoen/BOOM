// Project BOOM


#include "Character/BOOMCharacterMovementComponent.h"
#include "Character/BOOMCharacter.h"
#include "Components/CapsuleComponent.h"

UBOOMCharacterMovementComponent::UBOOMCharacterMovementComponent()
{
    MaxMantleHorizontalReach = 500.f;
    MaxMantleVerticalReach = 10.f;
    MinimumMantleSteepnessAngle = 77.f;
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


    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(Character);
    FHitResult HitResultFront;

    //@TODO: make class member - affects side surface detection precision
    int NumTraceQueries = 10;
    float TraceHeightIncrement = (CapsuleHalfHeight * 2) / NumTraceQueries;

    //Conduct Raycasts to find side of a mantleable surface.
    for (int i = 0; i < NumTraceQueries; i++)
    {
        StartTrace += FVector::DownVector * TraceHeightIncrement;
        EndTrace = StartTrace + (Character->GetActorForwardVector() * MaxMantleHorizontalReach);

        //DETECT SIDE SURFACE IN FRONT OF CHARACTER
        bool bHitFront = GetWorld()->LineTraceSingleByChannel(HitResultFront, StartTrace, EndTrace, ECC_Visibility, TraceParams);

        if (!bHitFront)
        {
            DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true);
            continue;
        }
        else
        {
            DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, true);
        }

        //@TODO: Add code to detect the steepness of the surface to see if it is climbable. Take dot product of normal up vector, then get the angle to determine.

        
        float UpDotNormal = FMath::Abs(FVector::DotProduct(HitResultFront.Normal, FVector::UpVector));
        //UE_LOG(LogTemp, Warning, TEXT("Dot Product %f."), UpDotNormal);

        //UE_LOG(LogTemp, Warning, TEXT("Normal Vector %s."), *HitResultFront.Normal.ToString());

        //UE_LOG(LogTemp, Warning, TEXT("Normal Magnitude %f."), HitResultFront.Normal.Length());
        //UE_LOG(LogTemp, Warning, TEXT("Up Vector Magnitude %f."), FVector::UpVector.Length());

        float FrontSteepnessRadians = FMath::Acos(UpDotNormal);

        float FrontSteepnessAngle = FMath::RadiansToDegrees(FrontSteepnessRadians);

        //UE_LOG(LogTemp, Warning, TEXT("Steepness Angle: %f."), FrontSteepnessAngle);

        if (( FMath::Abs(FrontSteepnessAngle)) < MinimumMantleSteepnessAngle)
        {
            UE_LOG(LogTemp, Warning, TEXT("Trace Failed Steepness Check."))
            DrawDebugLine(GetWorld(), StartTrace, EndTrace * 50, FColor::Orange, true);

            continue;
        }




        //SEE IF THE SURFACE IS BIG ENOUGH TO PHYSICALLY GRAB ONTO AND STAND UP ON, Otherwise, mantle over instead of onto.
        FVector BackTraceReference = CapsuleRadius * Character->GetActorForwardVector() + HitResultFront.Location;

        FHitResult HitResultBack;
        bool bHitBack = GetWorld()->LineTraceSingleByChannel(HitResultBack, HitResultFront.Location, BackTraceReference, ECC_Visibility, TraceParams);
        DrawDebugLine(GetWorld(), HitResultFront.Location, BackTraceReference, FColor::Emerald, true);

        if (!bHitBack)
        {
            continue;
        }

        //FIND THE POINT WHERE WE CAN CLIMB OR STEP UP ONTO

        //Finds top of climbable surface (the part we stand on) with vector math!
        TArray<FHitResult> HitResultsTop;

        /*Projects vector onto plane's normal vector, then uses vector subtraction to find vector on plane.*/
        FVector UpVectorProjectedOntoPlaneResult = FVector::VectorPlaneProject(FVector::UpVector, HitResultFront.Normal);


        FVector StartTraceTop = (UpVectorProjectedOntoPlaneResult * 2 * CapsuleHalfHeight) + HitResultFront.Location;
        FVector EndTraceTop = StartTraceTop - (UpVectorProjectedOntoPlaneResult * CapsuleHalfHeight * 2);


        bool bHitTop = GetWorld()->LineTraceMultiByChannel(HitResultsTop, StartTraceTop, EndTraceTop, ECC_Visibility, TraceParams);
        DrawDebugLine(GetWorld(), StartTraceTop, EndTraceTop, FColor::White, true);

        if (!bHitTop)
        {
            continue;
        }
        UE_LOG(LogTemp, Warning,TEXT("Hits from LineTraceMulti: %d"), HitResultsTop.Num())
        //RAYCAST IN DIRECTION OF PLANE PROJECTION  

    }
    return false;


}