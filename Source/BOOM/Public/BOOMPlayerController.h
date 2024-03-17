// Project BOOM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "BOOMPlayerController.generated.h"
/**
 * 
 */
UCLASS()
class BOOM_API ABOOMPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:

	class UBOOMPlayerHUD* GetPlayerHUD() const { return PlayerHUD; }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<class UBOOMPlayerHUD> PlayerHUDClass;

	UPROPERTY()
	class UBOOMPlayerHUD* PlayerHUD;
};
