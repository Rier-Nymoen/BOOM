// Project BOOM


#include "AI/BOOMPatrolRoute.h"

// Sets default values
ABOOMPatrolRoute::ABOOMPatrolRoute()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABOOMPatrolRoute::BeginPlay()
{
	Super::BeginPlay();
	//FTimerHandle PointHandle;
	//GetWorldTimerManager().SetTimer(PointHandle, this, &ABOOMPatrolRoute::FindNextPoint, 2.0F, true);
	//problem because wont count til after instances created. Cannot run in constructor.
	//if (Direction == EDirection::Ascending)
	//{
	//	CurrentPoint = 0;
	//}
	//else
	//{
	//	CurrentPoint = PatrolPoints.Num() - 1;
	//}
}

// Called every frame
void ABOOMPatrolRoute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABOOMPatrolRoute::FindNextPoint()
{

	//if (Direction == EDirection::Ascending)
	//{
	//	CurrentPoint++;
	//	if (CurrentPoint == PatrolPoints.Num() -1 )
	//	{
	//		Direction = EDirection::Descending;
	//	}
	//}
	//else
	//{
	//	CurrentPoint--;
	//	if (CurrentPoint == 0)
	//	{
	//		Direction = EDirection::Ascending;
	//	}
	//}

	
}
//in world space
const FVector ABOOMPatrolRoute::GetCurrentPoint()
{


	if (PatrolPoints.IsValidIndex(CurrentPoint))
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0F, FColor::Green, PatrolPoints[CurrentPoint].ToString() + " Index " + FString::FromInt(CurrentPoint));
		

		return	GetActorTransform().TransformPosition(PatrolPoints[CurrentPoint]);

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0F, FColor::Red, FVector::Zero().ToString() + " Index " + FString::FromInt(CurrentPoint));

		return FVector::Zero();
	}
}

