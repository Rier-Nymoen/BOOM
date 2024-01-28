// Project BOOM


#include "Character/BOOMCharacterMovementComponent.h"
#include "Character/BOOMCharacter.h"
#include "Components/CapsuleComponent.h"

UBOOMCharacterMovementComponent::UBOOMCharacterMovementComponent()
{
    MaxMantleHorizontalReach = 500.f;
    MaxMantleVerticalReach = 10.f;
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


        //SEE IF THE SURFACE IS BIG ENOUGH TO PHYSICALLY GRAB ONTO AND STAND UP ON
        FVector BackTraceReference = (-CapsuleRadius * HitResultFront.Normal + HitResultFront.Location);

        FHitResult HitResultBack;
        bool bHitBack = GetWorld()->LineTraceSingleByChannel(HitResultBack, HitResultFront.Location, BackTraceReference, ECC_Visibility, TraceParams);
        DrawDebugLine(GetWorld(), HitResultFront.Location, BackTraceReference, FColor::Emerald, true);

        if (!bHitBack)
        {
            continue;
        }

        //FIND THE POINT WHERE WE CAN CLIMB OR STEP UP ONTO

        //Top-Down raycast onto mantleable surface
        TArray<FHitResult> HitResultsTop;

        /*Projects vector onto plane's normal vector, then uses vector subtraction to find vector on plane.*/
        FVector UpVectorProjectedOntoPlaneResult = FVector::VectorPlaneProject(FVector::UpVector, HitResultFront.Normal);

        FVector StartTraceTop = (UpVectorProjectedOntoPlaneResult * 2 * CapsuleHalfHeight) + HitResultFront.Location;

        FVector EndTraceTop = StartTraceTop - (FVector::DownVector * CapsuleHalfHeight * 2);


        GetWorld()->LineTraceMultiByChannel(HitResultsTop, StartTraceTop, EndTraceTop, ECC_Visibility, TraceParams);
        DrawDebugLine(GetWorld(), StartTraceTop, EndTraceTop, FColor::White, true);



    }
    return false;


}