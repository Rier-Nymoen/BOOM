// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeapon.h"
#include "BOOMPickUpComponent.h"
#include "BOOM/BOOMCharacter.h"
#include "UI/BOOMPlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Weapons/BOOMWeaponReloadComponent.h"

/*
@TODO decide if it is worth transferring Weapon class into component
*/

// Sets default values
ABOOMWeapon::ABOOMWeapon()
{
	Weapon1P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh1P");
	BOOMPickUp = CreateDefaultSubobject<UBOOMPickUpComponent>("PickUpComponent");
	BOOMPickUp->SetupAttachment(Weapon1P);

}

// Called when the game starts or when spawned
void ABOOMWeapon::BeginPlay()
{
	Super::BeginPlay();
	bGenerateOverlapEventsDuringLevelStreaming = true;

	if (BOOMPickUp != nullptr)
	{

	}
}

void ABOOMWeapon::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.4F, FColor::Cyan, "ABOOMWeapon::Fire()");
}

void ABOOMWeapon::Interact()
{

}

void ABOOMWeapon::Interact(ABOOMCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}
	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	Character->SetHasRifle(true);
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(WeaponMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABOOMWeapon::Fire);
		}

	}


}

void ABOOMWeapon::OnInteractionRangeEntered(ABOOMCharacter* TargetCharacter)
{
	const  FString cont(TEXT("cont"));

	check(TargetCharacter)
	FItemInformation* MyStruct = TargetCharacter->WeaponTable->FindRow<FItemInformation>(Name, cont, true);

	if (MyStruct)
	{

		check(TargetCharacter->PlayerHUD)
		TargetCharacter->GetPlayerHUD()->PickUpPrompt->SetPromptImage(MyStruct->ItemImage);
		TargetCharacter->GetPlayerHUD()->PickUpPrompt->SetPromptText(Name);
		TargetCharacter->GetPlayerHUD()->PickUpPrompt->SetVisibility(ESlateVisibility::Visible);

	}
}

void ABOOMWeapon::OnInteractionRangeExited(ABOOMCharacter* TargetCharacter)
{
	check(TargetCharacter->GetPlayerHUD())
	TargetCharacter->GetPlayerHUD()->PickUpPrompt->SetVisibility(ESlateVisibility::Hidden);
}
