// Project BOOM


#include "Weapons/BOOMGrenade.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

ABOOMGrenade::ABOOMGrenade()
{
	ExplosionRadius = 510.f;
	GrenadeFuseTimeSeconds = 7.f;
	InitialLifeSpan = GrenadeFuseTimeSeconds;

}



void ABOOMGrenade::Explode()
{
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
	TArray<FHitResult> OutHitResults;

	UE_LOG(LogTemp, Warning, TEXT("oH"))

	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
	const bool bHit = GetWorld()->SweepMultiByChannel(OutHitResults, Hit.Location, Hit.Location, FQuat::Identity, ECC_Camera, SphereShape);

	DrawDebugSphere(GetWorld(), Hit.Location, ExplosionRadius, 50, FColor::Red, false, 1.f);

	if (bHit)
	{
		for (FHitResult& HitResult : OutHitResults)
		{

			UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(HitResult.GetActor()->GetRootComponent());

			if (MeshComponent)
			{
				MeshComponent->AddRadialImpulse(Hit.Location, ExplosionRadius, 10000.f, ERadialImpulseFalloff::RIF_Linear, true);
				//apply ges etc
			}

		}
	}

	Destroy();
}
