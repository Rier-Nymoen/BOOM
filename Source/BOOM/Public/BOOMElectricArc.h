// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BOOMElectricArc.generated.h"

UCLASS()
class BOOM_API ABOOMElectricArc : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABOOMElectricArc();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* DamageBox;

	class UMaterialInstance* ArcEffect;

	UBoxComponent* GetBoxComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
