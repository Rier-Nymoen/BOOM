// Project BOOM


#include "Character/BOOMCharacterMovementComponent.h"
#include "Character/BOOMCharacter.h"
#include "Components/CapsuleComponent.h"

UBOOMCharacterMovementComponent::UBOOMCharacterMovementComponent()
{
    MaxClimbingHorizontalReach = 500.f;
    MaxClimbingVerticalReach = 500.f;
}

void UBOOMCharacterMovementComponent::ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds)
{
    Super::ControlledCharacterMove(InputVector, DeltaSeconds);
}

bool UBOOMCharacterMovementComponent::DetectClimableSurface()
{

    /*
    Note: Consider the "walking mode step-up feature's thresholds"
    
    */
    ACharacter* Character = GetCharacterOwner();

    const float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    const float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius(); 

    //@TODO: make class member, get a better name.
    int NumTraceQueries = 10;

    float TraceHeightIncrement = (CapsuleHalfHeight * 2) / NumTraceQueries;


    
    FVector StartTrace = Character->GetActorLocation() - FVector(0,0,CapsuleHalfHeight);
    FVector EndTrace; 

    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(Character);
    
    FHitResult HitResult;

    for (int i = 0; i < NumTraceQueries; i++)
    {
        StartTrace += FVector(0, 0, TraceHeightIncrement);
        EndTrace = StartTrace + (Character->GetActorForwardVector() * MaxClimbingHorizontalReach);
        /*Find a normal for the side of a climbable surface*/
        bool bHitSurface = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, TraceParams);
        //UE_LOG(LogTemp, Warning, TEXT("Surface hit loc: %s"), *HitResult.Location.ToString())

        DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, true, 0.4);

        if (bHitSurface)
        {
            float DotProduct_Normal_W_Upwards = FVector::DotProduct(HitResult.Normal, FVector::UpVector);
            UE_LOG(LogTemp, Warning, TEXT("DotProduct: %f"), DotProduct_Normal_W_Upwards)
                /*Can get the angle between and tell if it's steep enough to mantle up.
 
                */
            DrawDebugLine(GetWorld(),  (-CapsuleRadius * HitResult.Normal  + HitResult.Location) , HitResult.Location, FColor::Cyan, true, 0.4);

            UE_LOG(LogTemp, Warning, TEXT("Normal Vector: %s"), *HitResult.Normal.ToString())
            FHitResult HitResultBack;
            /*Can use this hitscan to check if the suface length is big enough possibly.*/

            bool bHitBackSide = GetWorld()->LineTraceSingleByChannel(HitResultBack, (-CapsuleRadius * HitResult.Normal + HitResult.Location), HitResult.Location, ECC_Visibility, TraceParams);
            if (bHitBackSide)
            {

                //UE_LOG(LogTemp, Warning, TEXT("Back trace hit loc: %s"), *HitResultBack.Location.ToString())

                FVector DifferenceVector = HitResult.Location - HitResultBack.Location;
                UE_LOG(LogTemp, Warning, TEXT("Difference of front surface hit loc and back trace hit loc: % s"), *DifferenceVector.ToString())
            }
        }
        else
        {
            DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true, 0.4);
        }


        /*
        Shoot ray down to find surface we need to jump up onto.


        */



    }

    return false;
}
