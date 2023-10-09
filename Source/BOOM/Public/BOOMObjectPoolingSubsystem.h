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
private:
	//why UClass over TSubclassOf?
	class TMap<TWeakObjectPtr<UClass>, TArray<TWeakObjectPtr<AActor>>> ActorPools;

	//We only want to pool actors.
	void InitializeActorPool(TSubclassOf<AActor> ActorClass, int PoolSize);

	AActor* GetActor(TSubclassOf<AActor> ActorClass);
			
	//will use interface
};
