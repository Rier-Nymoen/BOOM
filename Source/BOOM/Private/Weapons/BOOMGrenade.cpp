// Project BOOM


#include "Weapons/BOOMGrenade.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include"AbilitySystemInterface.h"
#include "AI/BOOMAIController.h"

ABOOMGrenade::ABOOMGrenade()
{
	ExplosionRadius = 510.f;
	GrenadeFuseTimeSeconds = 4.f;
	InitialLifeSpan = 0.0f;

}

void ABOOMGrenade::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_GrenadeFuse, this, &ABOOMGrenade::Explode, GrenadeFuseTimeSeconds);

}


void ABOOMGrenade::Explode()
{
	TArray<FHitResult> OutHitResults;

	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
	const bool bHit = GetWorld()->SweepMultiByChannel(OutHitResults, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_Visibility, SphereShape);

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 10, FColor::Red, false, 1.f);

	if (bHit)
	{
		for (FHitResult& HitResult : OutHitResults)
		{
			UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(HitResult.GetActor()->GetRootComponent());

			if (MeshComponent)
			{
				MeshComponent->AddRadialImpulse(GetActorLocation(), ExplosionRadius, 10000.f, ERadialImpulseFalloff::RIF_Linear, true);
			}
		}
	}
	Destroy();
}

void ABOOMGrenade::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetCollisionComp()->MoveIgnoreActors.Add(GetInstigator());

}

void ABOOMGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void ABOOMGrenade::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	UE_LOG(LogTemp, Warning, TEXT("Grenade::GetOwnedGameplayTags"))
		TagContainer.AppendTags(GameplayTags);
}

