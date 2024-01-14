// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "BOOM/BOOMProjectile.h"
#include "BOOMGrenade.generated.h"

/**
 * 
 */
UCLASS()
class BOOM_API ABOOMGrenade : public ABOOMProjectile
{
	GENERATED_BODY()

public:

	ABOOMGrenade();


	struct FTimerHandle TimerHandle_GrenadeFuse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GrenadeFuseTimeSeconds;

	UPROPERTY(BlueprintReadWrite)
	struct FRuntimeFloatCurve ExplosionDamageFalloffCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionRadius;

	UFUNCTION()
	void Explode();

	virtual void ApplyThrownVelocity(FVector& ThrowDirection);

	UFUNCTION()
	virtual void PostInitializeComponents() override;

 	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	

};
