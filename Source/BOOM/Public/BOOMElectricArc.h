// Project BOOM

#pragma once

#include "Interfaces/PoolableObjectInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BOOMElectricArc.generated.h"

UCLASS()
class BOOM_API ABOOMElectricArc : public AActor, public IPoolableObjectInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABOOMElectricArc();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* DamageBox;

	class UMaterialInstance* ArcEffect;

	UBoxComponent* GetBoxComponent();

	virtual void InitActor() override;
	
	virtual void ReturnActorToPool() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
