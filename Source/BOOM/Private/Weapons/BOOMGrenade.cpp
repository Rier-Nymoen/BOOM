// Project BOOM


#include "Weapons/BOOMGrenade.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include"AbilitySystemInterface.h"


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

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 50, FColor::Red, false, 1.f);

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

void ABOOMGrenade::ApplyThrownVelocity(FVector& ThrowDirection)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = ThrowDirection; /** ProjectileMovement->InitialSpeed;*/

		UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *ProjectileMovement->Velocity.ToString())
	}
	
}

void ABOOMGrenade::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetCollisionComp()->MoveIgnoreActors.Add(GetInstigator());

}

void ABOOMGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//TArray<FHitResult> OutHitResults;

	//const FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
	//const bool bHit = GetWorld()->SweepMultiByChannel(OutHitResults, Hit.Location, Hit.Location, FQuat::Identity, ECC_Visibility, SphereShape);

	//DrawDebugSphere(GetWorld(), Hit.Location, ExplosionRadius, 50, FColor::Red, false, 1.f);

	//if (bHit)
	//{
	//	for (FHitResult& HitResult : OutHitResults)
	//	{
	//		
	//		UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(HitResult.GetActor()->GetRootComponent());
	//		
	//		if (MeshComponent)
	//		{
	//		
	//			MeshComponent->AddRadialImpulse(Hit.Location, ExplosionRadius, 10000.f, ERadialImpulseFalloff::RIF_Linear, true);
	//		}

	//	}
	//}

	//Destroy();
}
