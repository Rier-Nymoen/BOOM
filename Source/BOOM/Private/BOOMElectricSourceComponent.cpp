// Project BOOM


#include "BOOMElectricSourceComponent.h"

#include "BOOMElectricRadiusComponent.h"
#include "Math/UnitConversion.h"
#include "Util/IndexPriorityQueue.h"

UBOOMElectricSourceComponent::UBOOMElectricSourceComponent()
{
	Overlaps = 0;
	bIsActivated = true;
	RecalculateInterval = 0.3F;
	bHiddenInGame = true;
	SphereRadius = 250.F;
	ShapeColor = FColor(0, 60, 255);

}

void UBOOMElectricSourceComponent::BeginPlay()
{
	Super::BeginPlay();

	if(bIsActivated)
	{
		if(GetOwner())
		{
			GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_MST, this, &UBOOMElectricSourceComponent::MST, RecalculateInterval, bIsActivated);
			
		}
	}
}

/*
Prim's Algorithm Implementation - Creates a Minimum Spanning Tree -  https://en.wikipedia.org/wiki/Minimum_spanning_tree#:~:text=A%20minimum%20spanning%20tree%20(MST,minimum%20possible%20total%20edge%20weight.
*/

void UBOOMElectricSourceComponent::MST()
{
	GetOverlappingComponents(OverlappedComponents);

	TSet<UPrimitiveComponent*> Visited;
	TMap<UPrimitiveComponent*, float> DistanceMap;
	TArray<FPriorityQueueNode> PriorityQueue;
	TArray<FPriorityQueueNode> Path;
	
	FPriorityQueueNode StartNode(this, 0, nullptr);
	DistanceMap.Add(this, 0);
	PriorityQueue.HeapPush(StartNode, FMinDistancePredicate());

	float Infinity = TNumericLimits<float>::Max();

	while(PriorityQueue.Num() != 0)
	{
		FPriorityQueueNode CurrentNode;
		PriorityQueue.HeapPop(CurrentNode, FMinDistancePredicate());

		if(Visited.Find(CurrentNode.Component))
		{
			continue;
		}
		else
		{
			Path.Add(CurrentNode);
		}
		Visited.Add(CurrentNode.Component);

		TArray<UPrimitiveComponent*> Neighbors;

		CurrentNode.Component->GetOverlappingComponents(Neighbors);

		for (UPrimitiveComponent* Neighbor : Neighbors)
		{

			FHitResult HitResult;
			FCollisionQueryParams TraceParams;
			if (Neighbor->GetOwner())
			{
				TraceParams.AddIgnoredActor(CurrentNode.Component->GetOwner());

				TraceParams.AddIgnoredActor(Neighbor->GetOwner());
			}
			bool bPathIsObstructed = GetWorld()->LineTraceSingleByChannel(HitResult,
				CurrentNode.Component->GetComponentLocation(),
				Neighbor->GetComponentLocation(),
				ECC_Visibility,
				TraceParams);

			if(bPathIsObstructed)
			{
				DrawDebugLine(GetWorld(), CurrentNode.Component->GetComponentLocation(), HitResult.Location, FColor::Red, false, RecalculateInterval, 1, 3.F);
				continue;
			}
			float DistanceSquared = FVector::DistSquared(CurrentNode.Component->GetComponentLocation(), Neighbor->GetComponentLocation());

			if(!DistanceMap.Find(Neighbor))
			{
				DistanceMap.Add(Neighbor, Infinity);
			}

			if(!Visited.Find(Neighbor) && DistanceMap[Neighbor] > DistanceSquared)
			{
				FPriorityQueueNode NewNode(Neighbor, DistanceSquared, CurrentNode.Component);
				DistanceMap[Neighbor] = DistanceSquared;
				PriorityQueue.HeapPush(NewNode, FMinDistancePredicate());
			}
			
		}
	}
	ConnectMST(Path);
}

void UBOOMElectricSourceComponent::ConnectMST(TArray<FPriorityQueueNode> MSTResult)
{
	int i = 0;
	if(!MSTResult.IsEmpty())
	{
		while(i < MSTResult.Num())
		{
			if(MSTResult[i].ParentComponent != nullptr)
			{
				DrawDebugLine(GetWorld(), MSTResult[i].Component->GetComponentLocation(), MSTResult[i].ParentComponent->GetComponentLocation(), FColor::Blue, false, RecalculateInterval,1, 3.F);
			}
			i++;
		}
	}
}

