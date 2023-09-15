// Project BOOM


#include "BOOMElectricSourceComponent.h"

#include "BOOMElectricRadiusComponent.h"
#include "Interfaces/ElectricInterface.h"
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
	OnComponentBeginOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnSphereBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnSphereEndOverlap);
	MST();

		if(GetOwner())
		{
			GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_MST, this, &UBOOMElectricSourceComponent::CheckForUpdates, RecalculateInterval, true); //change bool
		}
	
}

/*
Prim's Algorithm Implementation - Creates a Minimum Spanning Tree -  https://en.wikipedia.org/wiki/Minimum_spanning_tree#:~:text=A%20minimum%20spanning%20tree%20(MST,minimum%20possible%20total%20edge%20weight.
*/
//If any component gets deleted from editor, can cause a major problem

//@TODO handle updating of MST when non-conducting nodes block paths. Probably get overlap of the connections between?
//@TOOD few bugs to handle but its looking okay
void UBOOMElectricSourceComponent::MST()
{
	for (TPair<UPrimitiveComponent*, FVector> Component : LastCheckedPosition)
	{

		Component.Key->OnComponentBeginOverlap.Clear();
	}

	LastCheckedPosition.Empty();

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

void UBOOMElectricSourceComponent::CheckForUpdates()
{

	if (LastCheckedPosition.IsEmpty())
	{
		return;
	}
	
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



void UBOOMElectricSourceComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.F, FColor::Blue, "rootoverlap");
	MST();
}

void UBOOMElectricSourceComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void UBOOMElectricSourceComponent::OnTest(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Orange, "MST Recalculated");
	MST();
}

void UBOOMElectricSourceComponent::OnTestEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, RecalculateInterval, FColor::Orange, "MST Recalculated");
	MST();
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

				UPrimitiveComponent* CurrentNode = MSTResult[i].Component;
				LastCheckedPosition.Add(CurrentNode, MSTResult[i].Position);

				//this can bind itself 1000+ times.. big no no.
				CurrentNode->OnComponentBeginOverlap.AddDynamic(this, &UBOOMElectricSourceComponent::OnTest);

				DrawDebugLine(GetWorld(), CurrentNode->GetComponentLocation(), MSTResult[i].ParentComponent->GetComponentLocation(), FColor::Blue, false, RecalculateInterval,1, 3.F);
			}
			i++;
		}
	}
}

