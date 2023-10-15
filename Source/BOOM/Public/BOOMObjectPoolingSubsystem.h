// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BOOMObjectPoolingSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API UBOOMObjectPoolingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:

	UBOOMObjectPoolingSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void InitializeActorPool(TSubclassOf<AActor> ActorClass, int PoolSize);

	AActor* GetActor(TSubclassOf<AActor> ActorClass); //O(N) retrieval

	TArray<TWeakObjectPtr<AActor>>* GetActorPool(TWeakObjectPtr<UClass> ActorClass);

private:

	class TMap<TWeakObjectPtr<UClass>, TArray<TWeakObjectPtr<AActor>>> ActorPools;
};
