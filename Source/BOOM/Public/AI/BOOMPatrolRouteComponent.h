// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BOOMPatrolRouteComponent.generated.h"

UENUM()
enum class EDirection : uint8 {

	Ascending,
	Descending
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOOM_API UBOOMPatrolRouteComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBOOMPatrolRouteComponent();


	UFUNCTION()
	virtual void FindNextPoint();

	UFUNCTION()
	const FVector GetCurrentPoint();

protected:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(VisibleAnywhere)
	int CurrentPoint;

	UPROPERTY(EditDefaultsOnly)
	EDirection Direction;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ABOOMPatrolRoute* PatrolRoute;

};
