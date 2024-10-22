// Copyright Epic Games, Inc. All Rights Reserved.

#include "BOOMProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ABOOMProjectile::ABOOMProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ABOOMProjectile::OnHit);		// set up a notification for when this component hits something blocking
	CollisionComp->SetSphereRadius(1.F);
	CollisionComp->bHiddenInGame = false;

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionComp);


	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 5.0f;
}

void ABOOMProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ABOOMProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//// Only add impulse and destroy projectile if we hit a physics
	//if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	//{
	//	//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	//	Destroy();
	//}
	

	//Destroy();
}

void ABOOMProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABOOMProjectile::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	UE_LOG(LogTemp,Warning,TEXT("GetOwnedGameplayTags"))
}
