 // Project BOOM


#include "BOOMPlayerController.h"
#include "UI/BOOMPlayerHUD.h"

void ABOOMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerHUD = CreateWidget<UBOOMPlayerHUD>(this, PlayerHUDClass);
	UE_LOG(LogTemp, Warning, TEXT(" ABOOMPlayerController::BeginPlay()"))

	if (PlayerHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("Added HUD to PlayerScreen"))
		PlayerHUD->AddToPlayerScreen();
	}
}
