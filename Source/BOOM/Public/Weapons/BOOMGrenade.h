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

	virtual void BeginPlay() override;

	struct FTimerHandle TimerHandle_GrenadeFuse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GrenadeFuseTimeSeconds;

	UPROPERTY(BlueprintReadWrite)
	struct FRuntimeFloatCurve ExplosionDamageFalloffCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionRadius;

	UFUNCTION()
	void Explode();

	UFUNCTION()
	virtual void PostInitializeComponents() override;

 	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	

};
