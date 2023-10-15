// Project BOOM


#include "BOOMElectricSourceComponent.h"

#include "BOOMElectricRadiusComponent.h"
#include "Interfaces/ElectricInterface.h"
#include "Interfaces/PoolableObjectInterface.h"
#include "Math/UnitConversion.h"
#include "Util/IndexPriorityQueue.h"
#include "BOOMElectricArc.h"
#include "Components/BoxComponent.h"
#include "BOOMObjectPoolingSubsystem.h"


/*
Most of this code is experimental and terribly unoptimized.
*/


/*
Optimizations Ideas: 

	- I don't have to recalculate all distances or  arcs in the ArcLists


*/

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
	//UBOOMObjectPoolingSubsystem* ObjectPoolingSubsystem = GetWorld()->GetSubsystem<UBOOMObjectPoolingSubsystem>();
	//ObjectPoolingSubsystem->InitializeActorPool(ABOOMElectricArc::StaticClass(), 20);

	if (GetOwner())
	{
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_MST, this, &UBOOMElectricSourceComponent::CheckForUpdates, RecalculateInterval, true);
	}
		 MST();
	
}

void UBOOMElectricSourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/*	
Prim's Algorithm Implementation - Creates a Minimum Spanning Tree -  https://en.wikipedia.org/wiki/Minimum_spanning_tree#:~:text=A%20minimum%20spanning%20tree%20(MST,minimum%20possible%20total%20edge%20weight.
*/


//NOTE: Need to make sure actors using the component have level streaming enabled

void UBOOMElectricSourceComponent::MST()
{
	TRACE_CPUPROFILER_EVENT_SCOPE("MST")
	GraphNodes.Empty(GraphNodes.Num());

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Orange, "MST Recalculated");

	//UBOOMObjectPoolingSubsystem* ObjectPoolingSubsystem = GetWorld()->GetSubsystem<UBOOMObjectPoolingSubsystem>();

	//for (int i = 0; i < ArcList.Num(); i++)
	//{

	//	TRACE_CPUPROFILER_EVENT_SCOPE("Restoring back to pool")
	//	//TWeakObjectPtr<AActor> ToActor = Cast<AActor>(ArcList[i]);

	//	/*@TODO: properly defined interface and options for sending to pool.*/
	//	//ObjectPoolingSubsystem->GetActorPool(ABOOMElectricArc::StaticClass())->Add(ToActor);
	//	//ArcList.RemoveAtSwap(i, 1, false);
	//}

	GetOverlappingComponents(OverlappedComponents);
	TSet<UPrimitiveComponent*> Visited;
	TMap<UPrimitiveComponent*, float> DistanceMap;
	TArray<FPriorityQueueNode> PriorityQueue;
	TArray<FPriorityQueueNode> MinimumSpanningTree;
	FCollisionQueryParams TraceParams;

	FPriorityQueueNode StartNode(this, 0, nullptr, this->GetComponentLocation());
	DistanceMap.Add(this, 0);
	PriorityQueue.HeapPush(StartNode, FMinDistancePredicate());

	float Infinity = TNumericLimits<float>::Max();

	while (PriorityQueue.Num() != 0)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE("Minheap Pop")

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
		TRACE_CPUPROFILER_EVENT_SCOPE("GetOverlappingComponents")

		CurrentNode.Component->GetOverlappingComponents(Neighbors);

		TRACE_CPUPROFILER_EVENT_SCOPE("Processing all neighbors")

		for (UPrimitiveComponent* Neighbor : Neighbors)
		{

			if (!GraphNodes.Find(Neighbor))
			{
				GraphNodes.Add(Neighbor, Neighbor->GetComponentLocation());
			}

			FHitResult HitResult;
			if (Neighbor->GetOwner())
			{
				TRACE_CPUPROFILER_EVENT_SCOPE("AddIgnoreActor")

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
				TRACE_CPUPROFILER_EVENT_SCOPE("HeapPush")
				FPriorityQueueNode NewNode(Neighbor, DistanceSquared, CurrentNode.Component, Neighbor->GetComponentLocation());
				DistanceMap[Neighbor] = DistanceSquared;

				PriorityQueue.HeapPush(NewNode, FMinDistancePredicate());
			}
		}
	}
	TRACE_CPUPROFILER_EVENT_SCOPE("AfterMSTCalculated")

	for(auto it = PoweredNodes.CreateConstIterator(); it; ++it)
	{
		UPrimitiveComponent* PoweredNode = *it;
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

			PoweredNode->OnComponentBeginOverlap.Remove(this, "OnGraphNodeBeginOverlap");
			if (IElectricInterface* ActiveObject = Cast<IElectricInterface>(PoweredNode->GetOwner()))
			{
				ActiveObject->OnDisconnectFromPower();
				
			}
			
		}
	}
	ConnectMST(MinimumSpanningTree);
}


void UBOOMElectricSourceComponent::CheckForUpdates()
{
	TRACE_CPUPROFILER_EVENT_SCOPE("CheckForUpdates")

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Cyan, "GraphNodes.Num = " + FString::FromInt(GraphNodes.Num()));
	for (TPair<UPrimitiveComponent*, FVector> Component : GraphNodes)
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

void UBOOMElectricSourceComponent::ConnectMST(TArray<FPriorityQueueNode> MinimumSpanningTree)
{
	TRACE_CPUPROFILER_EVENT_SCOPE("ConnectMST")
	FPriorityQueueNode CurrentNode;
	FActorSpawnParameters ActorSpawnParams;

	for (int i = 0; i < MinimumSpanningTree.Num(); i++) //assuming these nodes are valid.
	{

		CurrentNode = MinimumSpanningTree[i];

		if (CurrentNode.ParentComponent != nullptr)
		{

			//Spawns a box collision in between two points
			FVector Midpoint = (CurrentNode.ParentComponent->GetComponentLocation() + CurrentNode.Component->GetComponentLocation() ) /2;
			FVector Direction = CurrentNode.ParentComponent->GetComponentLocation() - CurrentNode.Component->GetComponentLocation();

			float Distance = FMath::Sqrt(CurrentNode.Cost);

			//UBOOMObjectPoolingSubsystem* ObjectPoolingSubsystem = GetWorld()->GetSubsystem<UBOOMObjectPoolingSubsystem>();


			////ABOOMElectricArc* ElectricArc = GetWorld()->SpawnActor<ABOOMElectricArc>(Arc, Midpoint, Direction.Rotation(), ActorSpawnParams);
			//
			//if (ABOOMElectricArc* ElectricArc = Cast<ABOOMElectricArc>(ObjectPoolingSubsystem->GetActor(ABOOMElectricArc::StaticClass())))
			//{
			//	TRACE_CPUPROFILER_EVENT_SCOPE("SquareRootOperation")

			//	//ElectricArc->SetActorLocation(Midpoint);
			//	//ElectricArc->SetActorRotation(Direction.Rotation());
			//	//UBoxComponent* ArcBoxVolume = ElectricArc->GetBoxComponent();

			//			float Distance = FMath::Sqrt(CurrentNode.Cost);


			//	////Change magic numbers to electric arc thickness or something.
			//	//ArcBoxVolume->SetBoxExtent(FVector(Distance / 2, 5, 5), false);

			//	//ArcList.Add(ElectricArc);
			//}			
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
}

void UBOOMElectricSourceComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TRACE_CPUPROFILER_EVENT_SCOPE("OnSphereNodeBeginOverlap")

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

	TRACE_CPUPROFILER_EVENT_SCOPE("OnGraphNodeBeginOverlap")

	if (PoweredNodes.Find(OtherComp))
	{

	}
	else
	{
		MST();
	}

}

