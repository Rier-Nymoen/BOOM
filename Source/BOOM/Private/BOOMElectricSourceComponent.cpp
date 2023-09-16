// Project BOOM


#include "BOOMElectricSourceComponent.h"

#include "BOOMElectricRadiusComponent.h"
#include "Interfaces/ElectricInterface.h"
#include "Math/UnitConversion.h"
#include "Util/IndexPriorityQueue.h"

UBOOMElectricSourceComponent::UBOOMElectricSourceComponent()
{
	Overlaps = 0;
	RecalculateInterval = 0.3F;
	bHiddenInGame = true;
	SphereRadius = 250.F;
	ShapeColor = FColor(0, 60, 255);
	bCanBeRecalculated = true;
}

void UBOOMElectricSourceComponent::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnSphereBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnSphereEndOverlap);
	MST();

		if(GetOwner())
		{
			GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_MST, this, &UBOOMElectricSourceComponent::CheckForUpdates, RecalculateInterval, true); //change bool after debugging.
		}
}

void UBOOMElectricSourceComponent::CheckForUpdates()
{
	bCanBeRecalculated = true;
	for (TPair<UPrimitiveComponent*, FVector> Component : LastCheckedPosition)
	{
		if (Component.Key->GetComponentLocation() == Component.Value)
		{
			continue;
		}
		else
		{
			MST();
			return;
		}
	}
}
/*	
Prim's Algorithm Implementation - Creates a Minimum Spanning Tree -  https://en.wikipedia.org/wiki/Minimum_spanning_tree#:~:text=A%20minimum%20spanning%20tree%20(MST,minimum%20possible%20total%20edge%20weight.
*/

/*
@TODO research if its worth using an incremental MST algorithm. Would love to optimize but time constraints exist.
@TODO note in this version objects that block connections dont update the MST... yet.
*/
//Need to make sure actors using the component have level streaming enabled
//bug if you cant calculate the MST the first try with whats overlapping, then you cant cant rebuild the tree until inserting a new node.
void UBOOMElectricSourceComponent::MST()
{
	//Realistically we will want to see if the components owner implements the interface.
	for (TPair<UPrimitiveComponent*, FVector> Component : LastCheckedPosition)
	{	
		if (!IsValid(Component.Key))
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 50.F, FColor::Red, "CAUGHT BAD CASE");
			continue;
		}
		Component.Key->OnComponentBeginOverlap.Clear();

		if (IElectricInterface* ElectricObject = Cast<IElectricInterface>(Component.Key))
		{
			ElectricObject->OnDisconnectFromPower();
		}
		
	}

	LastCheckedPosition.Empty();
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Orange, "MST Recalculated");

	GetOverlappingComponents(OverlappedComponents);
	TSet<UPrimitiveComponent*> Visited;
	TMap<UPrimitiveComponent*, float> DistanceMap;
	TArray<FPriorityQueueNode> PriorityQueue;
	TArray<FPriorityQueueNode> Path;
	FCollisionQueryParams TraceParams;
	
	FPriorityQueueNode StartNode(this, 0, nullptr, this->GetComponentLocation());
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
				FPriorityQueueNode NewNode(Neighbor, DistanceSquared, CurrentNode.Component, Neighbor->GetComponentLocation());
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
	if (MSTResult.IsEmpty())
	{
		return;
	}
	while (i < MSTResult.Num())
	{

		if (MSTResult[i].ParentComponent != nullptr)
		{
			UPrimitiveComponent* CurrentNode = MSTResult[i].Component;

			LastCheckedPosition.Add(CurrentNode, MSTResult[i].Position);

			CurrentNode->OnComponentBeginOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnTest);

			DrawDebugLine(GetWorld(), CurrentNode->GetComponentLocation(), MSTResult[i].ParentComponent->GetComponentLocation(), FColor::Blue, false, RecalculateInterval, 1, 3.F);

			if (IElectricInterface* ElectricObject = Cast<IElectricInterface>(CurrentNode))
			{
				ElectricObject->OnConnectToPower();
			}
		}
		i++;
	}

}


void UBOOMElectricSourceComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCanBeRecalculated)
	{
		bCanBeRecalculated = false;
		MST();
	}
}

void UBOOMElectricSourceComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void UBOOMElectricSourceComponent::OnTest(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCanBeRecalculated)
	{
		bCanBeRecalculated = false;
		MST();
	}
}



