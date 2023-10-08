// Project BOOM


#include "BOOMElectricSourceComponent.h"

#include "BOOMElectricRadiusComponent.h"
#include "Interfaces/ElectricInterface.h"
#include "Math/UnitConversion.h"
#include "Util/IndexPriorityQueue.h"
#include "BOOMElectricArc.h"
#include "Components/BoxComponent.h"

UBOOMElectricSourceComponent::UBOOMElectricSourceComponent()
{
	RecalculateInterval = 0.3F;
	bHiddenInGame = true;
	SphereRadius = 250.F;
	ShapeColor = FColor(0, 60, 255);
	bCanBeRecalculated = true;
	PrimaryComponentTick.bCanEverTick = false;

	GraphNodes.Reserve(100);
}

void UBOOMElectricSourceComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverlaps();
	GetOverlappingComponents(OverlappedComponents);
	OnComponentBeginOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnSphereBeginOverlap);

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.F, FColor::Cyan, FString::FromInt(OverlappedComponents.Num()));

	if (GetOwner())
	{
		//GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_MST, this, &UBOOMElectricSourceComponent::CheckForUpdates, RecalculateInterval, true); //change bool after debugging.
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_MST, this, &UBOOMElectricSourceComponent::CheckForUpdates, RecalculateInterval, false);

		//GetOwner()->GetWorldTimerManager().PauseTimer(TimerHandle_MST);
	}
		MST();
	
}

void UBOOMElectricSourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GetOverlappingComponents(OverlappedComponents);

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.F, FColor::Green, "Overlapped actors: " +  FString::FromInt(OverlappedComponents.Num()));
}

/*	
Prim's Algorithm Implementation - Creates a Minimum Spanning Tree -  https://en.wikipedia.org/wiki/Minimum_spanning_tree#:~:text=A%20minimum%20spanning%20tree%20(MST,minimum%20possible%20total%20edge%20weight.
*/

//@TODO Object pooling for spawning in electric effects and hitboxes when connecting the MST.

//Need to make sure actors using the component have level streaming enabled

void UBOOMElectricSourceComponent::MST()
{
	//GetOwner()->GetWorldTimerManager().PauseTimer(TimerHandle_MST);
	Visited.Empty(Visited.Num());
	GraphNodes.Empty(GraphNodes.Num());
	DistanceMap.Empty(DistanceMap.Num());
	MinimumSpanningTree.Empty(MinimumSpanningTree.Num());
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Orange, "MST Recalculated");

	GetOverlappingComponents(OverlappedComponents);
	//TSet<UPrimitiveComponent*> Visited;
	//TMap<UPrimitiveComponent*, float> DistanceMap;
	//TArray<FPriorityQueueNode> PriorityQueue;
	//TArray<FPriorityQueueNode> MinimumSpanningTree;
	FCollisionQueryParams TraceParams;

	FPriorityQueueNode StartNode(this, 0, nullptr, this->GetComponentLocation(), TArray<UPrimitiveComponent*>());
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
			MinimumSpanningTree.Add(CurrentNode);
		}
		Visited.Add(CurrentNode.Component);

		TArray<UPrimitiveComponent*> Neighbors;

		CurrentNode.Component->GetOverlappingComponents(Neighbors);

		for (UPrimitiveComponent* Neighbor : Neighbors)
		{

			if (!GraphNodes.Find(Neighbor))
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
				FPriorityQueueNode NewNode(Neighbor, DistanceSquared, CurrentNode.Component, Neighbor->GetComponentLocation(), TArray<UPrimitiveComponent*>());
				DistanceMap[Neighbor] = DistanceSquared;
				PriorityQueue.HeapPush(NewNode, FMinDistancePredicate());
			}
		}
	}

	for (UPrimitiveComponent* PoweredNode : PoweredNodes)
	{
		if (!IsValid(PoweredNode))
		{
			PoweredNodes.Remove(PoweredNode);
			continue;
		}
		if (Visited.Find(PoweredNode))
		{
			continue;
		}
		else
		{
			PoweredNodes.Remove(PoweredNode);

			//powered node shouldnt be ours i guess??
			PoweredNode->OnComponentBeginOverlap.Remove(this, "OnGraphNodeBeginOverlap");
			if (IElectricInterface* ActiveObject = Cast<IElectricInterface>(PoweredNode->GetOwner()))
			{
				ActiveObject->OnDisconnectFromPower();
				
			}
			
		}
	}

	ConnectMST(/*MinimumSpanningTree*/);
}


void UBOOMElectricSourceComponent::CheckForUpdates()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Cyan, "GraphNodes.Num = " + FString::FromInt(GraphNodes.Num()));
	for (TPair<UPrimitiveComponent*, FVector> Component : GraphNodes) //consider making it a const reference is what intellisense said?
	{
		if (IsValid(Component.Key) && Component.Key->GetComponentLocation() == Component.Value)
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

void UBOOMElectricSourceComponent::ConnectMST(/*TArray<FPriorityQueueNode> MinimumSpanningTree*/)
{
	FPriorityQueueNode CurrentNode;

	for (int i = 0; i < MinimumSpanningTree.Num(); i++) //assuming these nodes are valid.
	{

		CurrentNode = MinimumSpanningTree[i];

		if (CurrentNode.ParentComponent != nullptr)
		{

			//Spawns a box collision in between two points
			FActorSpawnParameters ActorSpawnParams;
			FVector Midpoint = (CurrentNode.ParentComponent->GetComponentLocation() + CurrentNode.Component->GetComponentLocation() ) /2;
			FVector Direction = CurrentNode.ParentComponent->GetComponentLocation() - CurrentNode.Component->GetComponentLocation();
			ABOOMElectricArc* ElectricArc = GetWorld()->SpawnActor<ABOOMElectricArc>(Arc, Midpoint, Direction.Rotation(), ActorSpawnParams);
			
			UBoxComponent* ArcBoxVolume =  ElectricArc->GetBoxComponent();

			float Distance = FMath::Sqrt(CurrentNode.Cost);
			//Change magic numbers to electric arc thickness or something.
			ArcBoxVolume->SetBoxExtent(FVector(Distance/2,5,5), true);


			// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.F, FColor::Blue, "Distance is : " + FString::SanitizeFloat(Distance));
			
			DrawDebugLine(GetWorld(), CurrentNode.Component->GetComponentLocation(), CurrentNode.ParentComponent->GetComponentLocation(), FColor::Blue, false, RecalculateInterval, 1, 3.F);
			if (!PoweredNodes.Find(CurrentNode.Component))
			{
				if (!IsValid(CurrentNode.Component))
				{
					return;
				}
				PoweredNodes.Add(CurrentNode.Component);

				CurrentNode.Component->OnComponentBeginOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnGraphNodeBeginOverlap);
				if (IElectricInterface* ConductingObject = Cast<IElectricInterface>(CurrentNode.Component->GetOwner()))
				{
					ConductingObject->OnConnectToPower();
				}
			}
			
		}
	}
	//if (GetOwner())
	//{
	//	GetOwner()->GetWorldTimerManager().UnPauseTimer(TimerHandle_MST);
	//}
}

void UBOOMElectricSourceComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PoweredNodes.Find(OtherComp))
	{

	}
	else
	{
		MST();//really should just signal its fine to do a calculation.
	}
}


void UBOOMElectricSourceComponent::OnGraphNodeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PoweredNodes.Find(OtherComp))
	{

	}
	else
	{
		MST();
	}

}

