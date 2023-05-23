// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeapon.h"
#include "BOOMPickUpComponent.h"
#include "BOOM/BOOMCharacter.h"
#include "UI/BOOMPlayerHUD.h"
#include "Kismet/GameplayStatics.h"

/*
@TODO decide if it is worth transferring Weapon class into component
*/

// Sets default values
ABOOMWeapon::ABOOMWeapon()
{
	Weapon1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P"));
	BOOMPickUp = CreateDefaultSubobject<UBOOMPickUpComponent>(TEXT("PickUpComponent"));
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

}

void ABOOMWeapon::Interact()
{

}

void ABOOMWeapon::Interact(ABOOMCharacter* MyCharacter)
{
	if (MyCharacter == nullptr)
	{
		return;
	}
	MyCharacter->EquipWeapon(this);
	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(MyCharacter->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

}

void ABOOMWeapon::OnInteractionRangeEntered(ABOOMCharacter* MyCharacter)
{
	const  FString cont(TEXT("cont"));

	check(MyCharacter)
		FItemInformation* MyStruct = MyCharacter->WeaponTable->FindRow<FItemInformation>(Name, cont, true);
	if (MyStruct)
	{

		check(MyCharacter->PlayerHUD)
		MyCharacter->GetPlayerHUD()->PickUpPrompt->SetPromptImage(MyStruct->ItemImage);
		MyCharacter->GetPlayerHUD()->PickUpPrompt->SetPromptText(Name);
		MyCharacter->GetPlayerHUD()->PickUpPrompt->SetVisibility(ESlateVisibility::Visible);

	}
}

void ABOOMWeapon::OnInteractionRangeExited(ABOOMCharacter* MyCharacter)
{
	check(MyCharacter->GetPlayerHUD())
	MyCharacter->GetPlayerHUD()->PickUpPrompt->SetVisibility(ESlateVisibility::Hidden);

}
