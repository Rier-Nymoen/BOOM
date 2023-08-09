// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BOOMElectricGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMElectricGraphNode : public UObject
{
	GENERATED_BODY()
public:

	bool bIsSource;
	bool bIsConnected;



	UPROPERTY()
	AActor* NodeActor;

	UPROPERTY()
	TSet<AActor*> IncludedVertices;

	UPROPERTY()
		TSet<AActor*> PendingInclusionVertices;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TEST)
		bool bTestStartAsSource;
};
