// Project BOOM


#include "BOOMElectricSource.h"

#include "BOOMElectricRadiusComponent.h"
#include "BOOMWeightedGraph.h"
#include "Util/IndexPriorityQueue.h"

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
	DebugMSTInterval = 1.0F;

}

// Called when the game starts or when spawned
void ABOOMElectricSource::BeginPlay()
{
	Super::BeginPlay();


	if (bIsPowerSource)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_MST, this, &ABOOMElectricSource::MST, DebugMSTInterval, true);
	}

}

// Called every frame
void ABOOMElectricSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GetOverlappingActors(OverlappedActors);


}

/*

original minheap route but didn't want to create my own minheap just for DecreaseKey Functionality to be supported.
*/

//void ABOOMElectricSource::MST()
//{
//
//	GetOverlappingActors(OverlappedActors);
//	Overlaps += OverlappedActors.Num();
//	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0F, FColor(252, 61, 3), "Overlapped Actors On Spawn: " + FString::FromInt(Overlaps));
//
//
//	TSet<AActor*> Visited;
//	TSet<AActor*> UnVisited;
//	
//	UnVisited.Add(this);
//
//
//	float Infinity = TNumericLimits<float>::Max();
//	TArray<FPriorityQueueNode> PriorityQueue;
//	//Add Self to Priority queue with distance of 0;
//
//	FPriorityQueueNode StartNode(this, 0);
//	PriorityQueue.HeapPush(StartNode, FMinDistancePredicate()); //can get index....
//
//
//	while (PriorityQueue.Num() != 0)
//	{
//
//		FPriorityQueueNode CurrentNode;
//		PriorityQueue.HeapPop(CurrentNode, FMinDistancePredicate());
//		Visited.Add(CurrentNode.Actor);
//
//		TArray<AActor*> Neighbors;
//
//		CurrentNode.Actor->GetOverlappingActors(Neighbors);
//		for (AActor* Neighbor : Neighbors)
//		{
//			//not in Minheap
//			if (!Visited.Find(Neighbor))
//			{
//				FPriorityQueueNode NewNode(Neighbor, Infinity);
//				int VertexPosition = PriorityQueue.HeapPush(NewNode, FMinDistancePredicate());
//				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Cyan, FString::FromInt(VertexPosition));
//
//			}
//			else
//			{
//				
//
//				float Distance = FVector::Distance(CurrentNode.Actor->GetActorLocation(), Neighbor->GetActorLocation());
//			}
//		}
//	
//
//
//	}
//
//
//}


/*
Prim's Algorithm Implementation - Creates a Minimum Spanning Tree -  https://en.wikipedia.org/wiki/Minimum_spanning_tree#:~:text=A%20minimum%20spanning%20tree%20(MST,minimum%20possible%20total%20edge%20weight.
*/


/*

Right now this operates on a time interval, but optimizations will be made when the system gets further developed. 

Can try to make MST generation event based such as player interaction, delegate, and overlaps

*/

void ABOOMElectricSource::MST()
{

	GetOverlappingActors(OverlappedActors);
	//Overlaps += OverlappedActors.Num();
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0F, FColor(252, 61, 3), "Overlapped Actors On Spawn: " + FString::FromInt(Overlaps));

	/*
	Shouldn't create and destroy memory every second
	*/
	TSet<AActor*> Visited;
	TMap<AActor*, float> DistanceMap;
	TArray<FPriorityQueueNode> MSTResult;

	TArray<FPriorityQueueNode> PriorityQueue;
	//Add Self to Priority queue with distance of 0;

	FPriorityQueueNode StartNode(this, 0, nullptr);
	DistanceMap.Add(this, 0);
	PriorityQueue.HeapPush(StartNode, FMinDistancePredicate()); 

	float Infinity = TNumericLimits<float>::Max();
	while (PriorityQueue.Num() != 0)
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3120.0F, FColor(60, 255, 133), "Iteration:");

		FPriorityQueueNode CurrentNode;
		PriorityQueue.HeapPop(CurrentNode, FMinDistancePredicate());


		if (Visited.Find(CurrentNode.Actor))
		{
			//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3102.0F, FColor(60, 11, 133), "Continued.");

			continue;
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3102.0F, FColor(60, 160, 133), "Actor: " + CurrentNode.Actor->GetActorLocation().ToString() + "Not found");

			if (CurrentNode.Parent != nullptr)
			{
				//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3102.0F, FColor(60, 11, 133), "Its Parent is: " + CurrentNode.Parent->GetActorLocation().ToString());
			}
			MSTResult.Add(CurrentNode);

		}
		Visited.Add(CurrentNode.Actor);

		TArray<AActor*> Neighbors;

		CurrentNode.Actor->GetOverlappingActors(Neighbors);
		for (AActor* Neighbor : Neighbors)
		{

			//Path is obstructed if an electric connection is impeded by non-conductors.
			FHitResult HitResult;
			FCollisionQueryParams TraceParams;
			bool bPathIsObstructed = GetWorld()->LineTraceSingleByChannel(HitResult, CurrentNode.Actor->GetActorLocation(), Neighbor->GetActorLocation(), ECC_Visibility, TraceParams);

			if (bPathIsObstructed)
			{
				DrawDebugLine(GetWorld(), CurrentNode.Actor->GetActorLocation(), HitResult.Location, FColor::Red, false, DebugMSTInterval, 1, 3.F);
				continue; 
			}

			



			float Distance = FVector::Distance(CurrentNode.Actor->GetActorLocation(), Neighbor->GetActorLocation());

			if (!DistanceMap.Find(Neighbor))
			{
				//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3012.0F, FColor(133, 115, 233), "Assigned Infinity Initially.");
				DistanceMap.Add(Neighbor, Infinity);
			}

			if (!Visited.Find(Neighbor) &&  DistanceMap[Neighbor] > Distance)
			{
				FPriorityQueueNode NewNode(Neighbor, Distance, CurrentNode.Actor);
				DistanceMap[Neighbor] = Distance;
				PriorityQueue.HeapPush(NewNode, FMinDistancePredicate());
				//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3102.0F, FColor(123, 161, 133), "Neighbor Added: " + Neighbor->GetActorLocation().ToString());

			}
			
		}

	}

	//GetWorldTimerManager().SetTimer(TimerHandle_DrawMST, this, &ABOOMElectricSource::DrawMST, 3.F, false);
	DrawMST(MSTResult);
	
}

void ABOOMElectricSource::DrawMST(TArray<FPriorityQueueNode> MSTResult)
{
	int i = 0;
	if (!MSTResult.IsEmpty())
	{
		while (i < MSTResult.Num()) //probably switch to iterator
		{

			if (MSTResult[i].Parent != nullptr)
			{

				DrawDebugLine(GetWorld(), MSTResult[i].Actor->GetActorLocation(), MSTResult[i].Parent->GetActorLocation(), FColor::Blue, false, DebugMSTInterval,1, 3.F);
			}

			

			//GetWorldTimerManager().SetTimer(TimerHandle_DrawMST, this, &ABOOMElectricSource::DrawMST, 0.3F, false);
			i++;
		}
		MSTResult.Empty();
	}
}
