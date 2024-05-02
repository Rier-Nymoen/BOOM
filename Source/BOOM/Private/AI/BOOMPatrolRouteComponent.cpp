// Project BOOM


#include "AI/BOOMPatrolRouteComponent.h"
#include "AI/BOOMPatrolRoute.h"

// Sets default values for this component's properties
UBOOMPatrolRouteComponent::UBOOMPatrolRouteComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	if (Direction == EDirection::Ascending)
	{
		CurrentPoint = 0;
	}

	if (PatrolRoute)
	{
		CurrentPoint = PatrolRoute->PatrolPoints.Num() - 1;
	}
	
	// ...
}


// Called when the game starts
void UBOOMPatrolRouteComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBOOMPatrolRouteComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBOOMPatrolRouteComponent::FindNextPoint()
{
	if (!PatrolRoute)
	{
		return;
	}

	if (Direction == EDirection::Ascending)
	{
		CurrentPoint++;
		if (CurrentPoint == PatrolRoute->PatrolPoints.Num() - 1)
		{
			Direction = EDirection::Descending;
		}
	}
	else
	{
		CurrentPoint--;
		if (CurrentPoint == 0)
		{
			Direction = EDirection::Ascending;
		}
	}
}

const FVector UBOOMPatrolRouteComponent::GetCurrentPoint()
{
	if (PatrolRoute && PatrolRoute->PatrolPoints.IsValidIndex(CurrentPoint))
	{
		UE_LOG(LogTemp, Warning, TEXT(" PatrolRoute->PatrolPoints[CurrentPoint]: %s"), *PatrolRoute->PatrolPoints[CurrentPoint].ToString())
		return PatrolRoute->GetActorLocation() + PatrolRoute->PatrolPoints[CurrentPoint];
	}
	return FVector::Zero();
	
}

