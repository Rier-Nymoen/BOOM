// Project BOOM


#include "BOOMObjectPoolingSubsystem.h"
#include "BOOMElectricArc.h"
UBOOMObjectPoolingSubsystem::UBOOMObjectPoolingSubsystem()
{
}

void UBOOMObjectPoolingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Warning, TEXT("UBOOMObjectPoolingSubsystem::Initialize"));

}

void UBOOMObjectPoolingSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UBOOMObjectPoolingSubsystem::InitializeActorPool(TSubclassOf<AActor> ActorClass, int PoolSize)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to create pool"));

	if(ActorPools.Find(ActorClass.Get()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Pool already exists for ActorClass:"));
		return;
	}

	
	TArray<TWeakObjectPtr<AActor>> ActorPool;
	
	FVector Origin = FVector::ZeroVector;
	if(UWorld* World = GetWorld())
	{
		FActorSpawnParameters ActorSpawnParams;
		for(int i = 0; i < PoolSize; i++)
		{
			TWeakObjectPtr<AActor> SpawnedActor =  World->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, ActorSpawnParams);
			
			ActorPool.Add(SpawnedActor);
		}		
	}
	//dont copy the array, add to pool set.
	ActorPools.Add(ActorClass.Get(), ActorPool);


	UE_LOG(LogTemp, Warning, TEXT("Pool created for ActorClass:"));

}

AActor* UBOOMObjectPoolingSubsystem::GetActor(TSubclassOf<AActor> ActorClass)
{

	TArray<TWeakObjectPtr<AActor>>* const SelectedPool = ActorPools.Find(ActorClass.Get());

	if(SelectedPool == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inside UBOOMObjectPoolingSubsystem::GetActor couldn't find classtype" ));
		return nullptr;
	}
	for (int i = 0; i < SelectedPool->Num(); i++)
	{
		AActor* ActorToSpawn = (*SelectedPool)[i].Get();

		if (ActorToSpawn != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Free Actor Found"))
			SelectedPool->RemoveAtSwap(i, 1, false);
			return ActorToSpawn;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Free actor not found."))
	return nullptr;	
}

TArray<TWeakObjectPtr<AActor>>* UBOOMObjectPoolingSubsystem::GetActorPool(TWeakObjectPtr<UClass> ActorClass)
{
	TArray<TWeakObjectPtr<AActor>>* ActorPool = ActorPools.Find(ActorClass);
	if (ActorPool)
	{
		return ActorPool;
	}
	return nullptr;
}
