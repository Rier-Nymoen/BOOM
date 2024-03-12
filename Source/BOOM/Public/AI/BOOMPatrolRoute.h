// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BOOMPatrolRoute.generated.h"

//UENUM()
//enum class EDirection : uint8 {
//
//	Ascending,
//	Descending
//};

UCLASS()
class BOOM_API ABOOMPatrolRoute : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABOOMPatrolRoute();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	//	EDirection Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (MakeEditWidget = true))
		TArray<FVector> PatrolPoints;

	UFUNCTION()
	virtual void FindNextPoint();

	UFUNCTION()
	const FVector GetCurrentPoint();

	UPROPERTY(EditAnywhere)
	int test;


	int CurrentPoint;

};
