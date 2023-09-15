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

	TArray<UPrimitiveComponent*> OverlappedComponents;

	UFUNCTION()
	void MST();

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = State)
	bool bIsActivated;

	UPROPERTY()
	FTimerHandle TimerHandle_ConnectMST;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecalculateInterval;

	UFUNCTION()
	void ConnectMST(TArray<FPriorityQueueNode> MSTResult);

	UFUNCTION()
	void CheckForUpdates();

	UPROPERTY()
	TMap<UPrimitiveComponent*, FVector> LastCheckedPosition;

	int Overlaps;

	UPROPERTY()
	FTimerHandle TimerHandle_MST;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION()
	void OnTest(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTestEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


};
