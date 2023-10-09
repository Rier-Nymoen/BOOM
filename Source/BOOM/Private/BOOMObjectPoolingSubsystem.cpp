// Project BOOM


#include "BOOMObjectPoolingSubsystem.h"
#include "BOOMElectricArc.h"
UBOOMObjectPoolingSubsystem::UBOOMObjectPoolingSubsystem()
{
}

void UBOOMObjectPoolingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UBOOMObjectPoolingSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UBOOMObjectPoolingSubsystem::InitializeActorPool(TSubclassOf<AActor> ActorClass, int PoolSize)
{

	if(ActorPools.Find(ActorClass->StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Pool already exists for ActorClass: %s"), FString(ActorClass->GetName()));
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
	ActorPools.Add(ActorClass->StaticClass(), ActorPool);
	UE_LOG(LogTemp, Warning, TEXT("Pool created for ActorClass: %s"), FString(ActorClass->GetName()));

}

AActor* UBOOMObjectPoolingSubsystem::GetActor(TSubclassOf<AActor> ActorClass)
{
	if(!ActorPools.Find(ActorClass))
	{
		return nullptr;
	}

	
	
}
