// Project BOOM


#include "Character/BOOMCharacterMovementComponent.h"
#include "Character/BOOMCharacter.h"
#include "Components/CapsuleComponent.h"

UBOOMCharacterMovementComponent::UBOOMCharacterMovementComponent()
{
    MaxClimbingHorizontalReach = 500.f;
    MaxClimbingVerticalReach = 10.f;
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
            DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true, 0.0);
            continue;
        }
        else
        {
            DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, true, 0.0);
        }

        FVector BackTraceReference = (-CapsuleRadius * HitResultFront.Normal + HitResultFront.Location);

        //SEE IF THE SURFACE IS BIG ENOUGH TO PHYSICALLY GRAB ONTO AND STAND UP ON
        FHitResult HitResultBack;
        bool bHitBack= GetWorld()->LineTraceSingleByChannel(HitResultBack, StartTrace, EndTrace, ECC_Visibility, TraceParams);

        if (!bHitBack)
        {
            continue;
        }
        //Top-Down raycast onto mantleable surface
        TArray<FHitResult> HitResultsTop;
       



    }


     return false;

