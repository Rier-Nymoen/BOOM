// Project BOOM


#include "BOOMElectricSource.h"

#include "BOOMElectricRadiusComponent.h"
#include "BOOMWeightedGraph.h"
// Sets default values
ABOOMElectricSource::ABOOMElectricSource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	ElectricalRadius = CreateDefaultSubobject<UBOOMElectricRadiusComponent>("ElectricRadius");
	ElectricalRadius->SetupAttachment(Mesh);
	Overlaps = 0;
	bIsPowerSource = false;
	bIsConnectedToPowerSource = bIsPowerSource;
	
}

// Called when the game starts or when spawned
void ABOOMElectricSource::BeginPlay()
{
	Super::BeginPlay();


	if (bIsPowerSource)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_MST, this, &ABOOMElectricSource::MST, 5.F, false);
	}

}

// Called every frame
void ABOOMElectricSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GetOverlappingActors(OverlappedActors);

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0F, FColor(252, 61, 3), "Connected: " + FString::FromInt(OverlappedActors.Num()));

}

void ABOOMElectricSource::MST()
{



	GetOverlappingActors(OverlappedActors);
	Overlaps += OverlappedActors.Num();
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0F, FColor(252, 61, 3), "Overlapped Actors On Spawn: " + FString::FromInt(Overlaps));


	TSet<AActor*> Visited;
	float MinDistance = TNumericLimits<float>::Max();
	TArray<FPriorityQueueNode> PriorityQueue;
	//Add Self to Priority queue with distance of 0;

	//Self is visited, has distance of 0
	FPriorityQueueNode SelfNode(this, 0);
	//PriorityQueue.HeapPush(SelfNode, FMinDistancePredicate());
	Visited.Add(SelfNode.Actor)	;
	
	for (AActor* VisitedActor : Visited)
	{
		TArray<AActor*> Neighbors;
		//GetNeighbors
		GetOverlappingActors(Neighbors);

		for (AActor* Neighbor : Neighbors)
		{
			if (Visited.Find(Neighbor))
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Emerald, "Already found neighbor, should ignore");
				continue;
			}
			//get cost to node from discovered node
			FPriorityQueueNode NewNode(Neighbor, FVector::Distance(VisitedActor->GetActorLocation(), Neighbor->GetActorLocation()));
			PriorityQueue.HeapPush(NewNode, FMinDistancePredicate());

		}

	}
	FPriorityQueueNode Elem;

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 40.F, FColor::Blue, "Size of priority queue" + FString::FromInt(PriorityQueue.Num()));

	PriorityQueue.HeapPop(Elem, FMinDistancePredicate());
	
	TArray<AActor*> ElemsNeighbors;
	Elem.Actor->GetOverlappingActors(ElemsNeighbors);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 40.F, FColor::Magenta, "Next Elems neighbor count" + FString::FromInt(ElemsNeighbors.Num()));


	//FPriorityQueueNode Elem;
	//while (PriorityQueue.Num() != 0)
	//{
	//	PriorityQueue.HeapPop(Elem, FMinDistancePredicate());
	//	//Should have only unique vertices added from the overlap actors into the set.
	//	

	//	if (Visited.Find(Elem.Actor))
	//	{
	//		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 50.0F, FColor(252, 61, 3), "Visited");

	//		continue;
	//	}

	//	//Smallest, edge but must check if it connects to another node.
	//	
	//	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 50.0F, FColor(252, 61, 3), "Costs in min order: " + FString::SanitizeFloat(Elem.Cost));


	//}




}

//for (AActor* Vertex : OverlappedActors)
//{
//	//if (Visited.Find(Vertex)) 
//	//{
//	//	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 50.0F, FColor(252, 61, 3), "Visited neighbor already ");

//	//	continue;
//	//}

//	FPriorityQueueNode Node(Vertex, FVector::Distance(this->GetActorLocation(), Vertex->GetActorLocation())); 


//	
//	PriorityQueue.HeapPush(Node, FMinDistancePredicate());
//	//TArray<AActor*> Neighbor;

//}
