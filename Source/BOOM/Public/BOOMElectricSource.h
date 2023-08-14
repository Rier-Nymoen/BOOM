// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BOOMElectricSource.generated.h"



USTRUCT()
struct FPriorityQueueNode
{
	GENERATED_BODY()


	FPriorityQueueNode()
	{

	}

	FPriorityQueueNode(AActor* InActor, float InCost, AActor* InParent)
	{
		Actor = InActor;
		Cost = InCost;
		Parent = InParent;
	}
	AActor* Actor;
	float Cost;
	AActor* Parent;
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



UCLASS()
class BOOM_API ABOOMElectricSource : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABOOMElectricSource();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Electric)
	class UBOOMElectricRadiusComponent* ElectricalRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	TArray<AActor*> OverlappedActors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Electric)
		bool bIsPowerSource;

	UPROPERTY()
		FTimerHandle TimerHandle_DrawMST;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DebugMSTInterval;

	UFUNCTION()
	void MST();

	UFUNCTION()
	void DrawMST(TArray<FPriorityQueueNode> MSTResult);

	bool bIsConnectedToPowerSource;

	int Overlaps;

	UPROPERTY()
	FTimerHandle TimerHandle_MST;

};
