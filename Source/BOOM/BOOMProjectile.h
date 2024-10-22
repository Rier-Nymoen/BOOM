// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h"
#include "BOOMProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class ABOOMProjectile : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
public:


	ABOOMProjectile();

	virtual void BeginPlay() override;

	/** Sphere collision component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	USphereComponent* CollisionComp;


	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UPROPERTY(EditAnywhere, Category = GameplayEffects)
	TSubclassOf<class UGameplayEffect> DamageEffect;
	
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	struct FGameplayTagContainer GameplayTags;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

};

