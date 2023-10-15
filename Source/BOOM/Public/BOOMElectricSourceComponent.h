// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BOOMElectricSourceComponent.generated.h"

/**
 * 
 */
class UBOOMElectricRadiusComponent;

USTRUCT()
struct FPriorityQueueNode
{
	GENERATED_BODY()

	FPriorityQueueNode()
	{

	}

	FPriorityQueueNode(UPrimitiveComponent* InComponent, float InCost, UPrimitiveComponent* InParentComponent, FVector InPosition)
	{
		Component = InComponent;
		Cost = InCost;
		ParentComponent = InParentComponent;
		Position = InPosition;
	}
	UPrimitiveComponent* Component;
	float Cost;
	UPrimitiveComponent* ParentComponent;
	FVector Position;
};

USTRUCT()
struct FMinDistancePredicate
{
	GENERATED_BODY()
	bool operator()(const FPriorityQueueNode& A, const FPriorityQueueNode& B) const
	{
		return A.Cost < B.Cost;
	}
};	

UCLASS(Blueprintable, ClassGroup=(Custom), meta = (BlueprintSpawnableComponent))
class BOOM_API UBOOMElectricSourceComponent : public USphereComponent
{
	GENERATED_BODY()

public:

	UBOOMElectricSourceComponent();

	UPROPERTY()
	TArray<UPrimitiveComponent*> OverlappedComponents;

	UFUNCTION()
	void MST();

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	UPROPERTY()
	FTimerHandle TimerHandle_ConnectMST;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecalculateInterval;

	UFUNCTION()
	void ConnectMST(TArray<FPriorityQueueNode> MinimumSpanningTree);

	UFUNCTION()
	void CheckForUpdates();

	UPROPERTY()
	TMap<UPrimitiveComponent*, FVector> LastCheckedPosition;

	UPROPERTY()
	FTimerHandle TimerHandle_MST;

	UPROPERTY()
	TSet<UPrimitiveComponent*> PoweredNodes;

	//TMap<UPrimitiveComponent*, UPrimitiveComponent*> PredecessorMap;
	//UPROPERTY()
	//TSet<UPrimitiveComponent*> Visited;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGraphNodeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//UPROPERTY()
	//TMap<UPrimitiveComponent*, float> DistanceMap;
	//UPROPERTY()
	//TArray<FPriorityQueueNode> PriorityQueue;
	//UPROPERTY()
	//TArray<FPriorityQueueNode> MinimumSpanningTree;
	UPROPERTY()
	TMap<UPrimitiveComponent*, FVector> GraphNodes;

	UPROPERTY(VisibleAnywhere)
	bool bCanBeRecalculated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> Arc;

	UPROPERTY()
	TArray<class ABOOMElectricArc*> ArcList;
};
