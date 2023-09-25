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

//@TODO Object pooling for spawning in electric effects and hitboxes when connecting the MST.

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
	TSet<UPrimitiveComponent*> NodesToUnpower;

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
			if (NodesToUnpower.Find(CurrentNode.Component))
			{
				NodesToUnpower.Remove(CurrentNode.Component);
			}
			//this is where I would remove nodes that could have to be powered down.
			
			PoweredNodes.Add(CurrentNode.Component);
			Path.Add(CurrentNode);
		}
		Visited.Add(CurrentNode.Component);

		TArray<UPrimitiveComponent*> Neighbors;

		CurrentNode.Component->GetOverlappingComponents(Neighbors);

		for (UPrimitiveComponent* Neighbor : Neighbors)
		{
			if (Neighbor == nullptr)
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.F, FColor::Red, "NuLLPTR NEIGHBOR");
				return;
			}
			if (!GraphNodes.Find(Neighbor))
			{
				Neighbor->OnComponentEndOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnGraphNodeEndOverlap);
				Neighbor->OnComponentBeginOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnGraphNodeBeginOverlap);
			}
			if (IsValid(Neighbor))
			{
				GraphNodes.Add(Neighbor, Neighbor->GetComponentLocation());
			}

			

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
				//Could Keep track of nodes that are overlapped but will may need to be removed. Essentially add a node to a pending list, and remove it at the end of the calculation if it doesn't make the cut.
				if (PoweredNodes.Find(Neighbor))
				{
					NodesToUnpower.Add(Neighbor);
				}
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

	//Check for nodes that are no longer powered (not in the result)

	for (UPrimitiveComponent* Node : NodesToUnpower)
	{
		if (!IsValid(Node))
		{
			continue;
		}
		RemoveNodePower(Node);
	}

	ConnectMST(Path);

}

void UBOOMElectricSourceComponent::ConnectMST(TArray<FPriorityQueueNode> MSTResult)
{
	if (GetOwner())
	{
		GetOwner()->GetWorldTimerManager().UnPauseTimer(TimerHandle_MST);
	}


	UPrimitiveComponent* CurrentNode;
	UPrimitiveComponent* ParentNode;

	for (int i = 0; i < MSTResult.Num(); i++)
	{
		CurrentNode = MSTResult[i].Component;
		ParentNode = MSTResult[i].ParentComponent;
		if (ParentNode != nullptr)
		{
			DrawDebugLine(GetWorld(), CurrentNode->GetComponentLocation(), MSTResult[i].ParentComponent->GetComponentLocation(), FColor::Blue, false, RecalculateInterval, 1, 3.F);
			PowerNode(CurrentNode);
		}
	}
	if (GetOwner())
	{
		GetOwner()->GetWorldTimerManager().UnPauseTimer(TimerHandle_MST);
	}

}

void UBOOMElectricSourceComponent::RemoveNodePower(UPrimitiveComponent* NodeToRemove)
{
	PoweredNodes.Remove(NodeToRemove);
	if (!IsValid(NodeToRemove))
	{
		return;
	}
	IElectricInterface* ElectricInterface = Cast<IElectricInterface>(NodeToRemove->GetOwner());
	if (ElectricInterface)
	{
		ElectricInterface->OnDisconnectFromPower();
	}
}

void UBOOMElectricSourceComponent::PowerNode(UPrimitiveComponent* Node)
{
	if (!IsValid(Node))
	{
		PoweredNodes.Remove(Node);
	}


	if (Node == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.F, FColor::Red, "nullptr node");

	}

	if (IElectricInterface* ElectricInterface = Cast<IElectricInterface>(Node->GetOwner()))
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.F, FColor::Emerald, "OnConnectPower");
		ElectricInterface->OnConnectToPower();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.F, FColor::Red, "NoGo");
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


void UBOOMElectricSourceComponent::OnGraphNodeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (GraphNodes.Find(OtherComp))
	{
		return;
	}
	MST();
	//we need to do a recalculation of the tree.

}


//@TODO 
/* 
* A bug occurs when an object adjacemt to a tree node remains overlapping, but is blocked by an obstacle. This causes the object to not be considered when
* it is moved.

To fix: need to store nodes that are blocked and also check if their positions changed.
*/
void UBOOMElectricSourceComponent::CheckForUpdates()
{
	for (TPair<UPrimitiveComponent*, FVector> Component : GraphNodes) //consider making it a const reference is what intellisense said?
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

void UBOOMElectricSourceComponent::OnGraphNodeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TArray<UPrimitiveComponent*> Neighbors;
	OverlappedComp->GetOverlappingComponents(Neighbors);
	for (UPrimitiveComponent* Neighbor : Neighbors) //IsValid checks possibly
	{
		if (GraphNodes.Find(Neighbor))
		{
			return;
		}
	}
	OverlappedComp->OnComponentBeginOverlap.Clear();
	GraphNodes.Remove(OverlappedComp);
	if (PoweredNodes.Find(OverlappedComp))
	{
		RemoveNodePower(OverlappedComp);
	}
	MST(); //Will remove anything not powered
}


/*

I could just set nodes in the mst to be powered
*/