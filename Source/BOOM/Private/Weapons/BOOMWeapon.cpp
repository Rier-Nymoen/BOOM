// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeapon.h"
#include "BOOMPickUpComponent.h"
#include "BOOM/BOOMCharacter.h"
#include "UI/BOOMPlayerHUD.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABOOMWeapon::ABOOMWeapon()
{
	Weapon1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P"));
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	BOOMPickUp = CreateDefaultSubobject<UBOOMPickUpComponent>(TEXT("PickUpComponent"));
	BOOMPickUp->SetupAttachment(Weapon1P);
}

// Called when the game starts or when spawned
void ABOOMWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (BOOMPickUp != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.0F, FColor::Cyan, "BOOMPICKUPEXISTS");

	}


}

void ABOOMWeapon::Fire()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.0F, FColor::Green, "PewPewPew");

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
	MyCharacter->SetCurrentWeapon(this);
	if (MyCharacter->GetCurrentWeapon() == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.0F, FColor::Cyan, "didnt work");

	}
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
			MyCharacter->PlayerHUD->PickUpPrompt->SetPromptImage(MyStruct->ItemImage);
		MyCharacter->PlayerHUD->PickUpPrompt->SetPromptText(Name);

	}
}
