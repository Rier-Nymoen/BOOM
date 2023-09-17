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
	
	GetOverlappingComponents(OverlappedComponents);
	if (OverlappedComponents.Num() > 0)
	{
		if (GetOwner())
		{
			GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_MST, this, &UBOOMElectricSourceComponent::CheckForUpdates, RecalculateInterval, true); //change bool after debugging.
			GetOwner()->GetWorldTimerManager().PauseTimer(TimerHandle_MST);
		}
		MST();

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

//NEED TO DO VALIDITY CHECKS FOR NODES IN THE TREE.
void UBOOMElectricSourceComponent::MST()
{
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

	while (PriorityQueue.Num() != 0)
	{
		FPriorityQueueNode CurrentNode;
		PriorityQueue.HeapPop(CurrentNode, FMinDistancePredicate());

		if (Visited.Find(CurrentNode.Component))
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

			if (bPathIsObstructed)
			{
				DrawDebugLine(GetWorld(), CurrentNode.Component->GetComponentLocation(), HitResult.Location, FColor::Red, false, RecalculateInterval, 1, 3.F);
				continue;
			}
			float DistanceSquared = FVector::DistSquared(CurrentNode.Component->GetComponentLocation(), Neighbor->GetComponentLocation());

			if (!DistanceMap.Find(Neighbor))
			{
				DistanceMap.Add(Neighbor, Infinity);
			}

			if (!Visited.Find(Neighbor) && DistanceMap[Neighbor] > DistanceSquared)
			{
				FPriorityQueueNode NewNode(Neighbor, DistanceSquared, CurrentNode.Component, Neighbor->GetComponentLocation());
				DistanceMap[Neighbor] = DistanceSquared;
				PriorityQueue.HeapPush(NewNode, FMinDistancePredicate());
			}
		}
	}

	for (TPair<UPrimitiveComponent*, FVector> Component : LastCheckedPosition)
	{
		//we could have components that will be marked for deletion
		if (!IsValid(Component.Key))
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Red, "CAUGHT BAD CASE");
			
			LastCheckedPosition.Remove(Component.Key);
			continue;
		}
		
		if (Visited.Find(Component.Key))
		{
			Component.Value = Component.Key->GetComponentLocation();
			continue;
		}
		else
		{
			Component.Key->OnComponentBeginOverlap.Clear();
			LastCheckedPosition.Remove(Component.Key);
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Orange, "A node was removed from the tree.");
			//Also disconnect from power.

			if (IElectricInterface* InterfaceObject = Cast<IElectricInterface>(Component.Key))
			{
				InterfaceObject->OnDisconnectFromPower();
			}


		}
	}


	ConnectMST(Path);

}

void UBOOMElectricSourceComponent::ConnectMST(TArray<FPriorityQueueNode> MSTResult)
{
	if (MSTResult.IsEmpty())
	{
		GetOwner()->GetWorldTimerManager().UnPauseTimer(TimerHandle_MST);

		return;
	}
	
	UPrimitiveComponent* CurrentNode;
	UPrimitiveComponent* ParentNode;
	for(int i = 0; i < MSTResult.Num(); i++)
	{
		if (MSTResult[i].ParentComponent != nullptr)
		{
			CurrentNode = MSTResult[i].Component;
			ParentNode = MSTResult[i].ParentComponent;

			if (LastCheckedPosition.Find(CurrentNode))
			{
				LastCheckedPosition[CurrentNode] = CurrentNode->GetComponentLocation();
				//GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Green, "Node is already apart of our tree");
				//already powered
			}
			else
			{
				CurrentNode->OnComponentBeginOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnTest);
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Yellow, "New node to be considered for the tree.");

				LastCheckedPosition.Add(CurrentNode);
				//needs to be powered

				if (IElectricInterface* InterfaceObject = Cast<IElectricInterface>(CurrentNode))
				{
					InterfaceObject->OnConnectToPower();
				}
			}
			DrawDebugLine(GetWorld(), CurrentNode->GetComponentLocation(), MSTResult[i].ParentComponent->GetComponentLocation(), FColor::Blue, false, RecalculateInterval, 1, 3.F);
		}
	}
	
	if (GetOwner())
	{
		GetOwner()->GetWorldTimerManager().UnPauseTimer(TimerHandle_MST);
	}
}

void UBOOMElectricSourceComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (LastCheckedPosition.Find(OtherComp))
	{
		return;
	}
	MST();

	//we need to do a recalculation of the tree.
}


void UBOOMElectricSourceComponent::OnTest(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (LastCheckedPosition.Find(OtherComp))
	{
		return;
	}
	MST();
	//we need to do a recalculation of the tree.

}

void UBOOMElectricSourceComponent::CheckForUpdates()
{
	for (TPair<UPrimitiveComponent*, FVector> Component : LastCheckedPosition)
	{
		if (Component.Key->GetComponentLocation() == Component.Value)
		{
			continue;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.F, FColor::Cyan, "checking for updates caused mst call");

			MST();
			return;
		}
	}
}

/*
When nodes apart of the graph generate an overlap event, we we need to check if the node is already in the graph or not. If it is, ignore it. If it isnt, its a new node and we want to calculate it.


When we calculate the new MST result, thats when we want to take the old result, and we want to remove nodes that are not in the MST result, from being ignored by component collisions, and distance updates.
*/


void UBOOMElectricSourceComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}