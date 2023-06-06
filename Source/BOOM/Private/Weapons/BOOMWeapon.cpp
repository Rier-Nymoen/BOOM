// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeapon.h"
#include "BOOMPickUpComponent.h"
#include "BOOM/BOOMCharacter.h"
#include "UI/BOOMPlayerHUD.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Weapons/BOOMWeaponStateActive.h"
#include "Weapons/BOOMWeaponStateInactive.h"
#include "Weapons/BOOMWeaponStateEquipping.h"
#include "Weapons/BOOMWeaponStateFiring.h"
#include "Weapons/BOOMWeaponStateReloading.h"
#include "Weapons/BOOMWeaponStateUnequipping.h"


/*
@TODO decide if it is worth transferring Weapon class into component

Weapon States are useful, all it is is having this set of classes handle a character's weapon interactions, rather than storing those states
inside of the character class, we handle weapon states with the weapons! all the player is doing, is supplying inputs.

If its character related things, handle it with the character states


Can handle input with state functions, or have a separate handling input. I will just use HandleFireInput

*/

// Sets default values
ABOOMWeapon::ABOOMWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	Weapon1P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh1P");
	BOOMPickUp = CreateDefaultSubobject<UBOOMPickUpComponent>("PickUpComponent");
	BOOMPickUp->SetupAttachment(Weapon1P);
	
	ActiveState = CreateDefaultSubobject<UBOOMWeaponStateActive>("ActiveState");
	InactiveState = CreateDefaultSubobject<UBOOMWeaponStateInactive>("InactiveState");
	FiringState = CreateDefaultSubobject<UBOOMWeaponStateFiring>("FiringState");
	ReloadingState = CreateDefaultSubobject<UBOOMWeaponStateReloading>("ReloadingState");
	EquippingState = CreateDefaultSubobject<UBOOMWeaponStateEquipping>("EquippingState");
	UnequippingState = CreateDefaultSubobject<UBOOMWeaponStateUnequipping>("UnequippingState");

	MaxAmmoReserves = 12;
	//Player will spawn in with the max ammo reserves at the start of the game
	CurrentAmmoReserves = MaxAmmoReserves;
	AmmoCost = 1;

	//Should be able to fire initially

	//Set size of magazine
	MagazineSize = 10;

	//Player will spawn with a full magazine
	CurrentAmmo = MagazineSize;

	//Time it takes for a magazine to be reloaded into the weapon
	ReloadDurationSeconds = 2.2f;

	HitscanRange = 2000.0F;
	WeaponDamage = 100.0F;
}

// Called when the game starts or when spawned
void ABOOMWeapon::BeginPlay()
{
	if (Character == nullptr)
	{
		CurrentState = InactiveState;
	}

	Super::BeginPlay();
	bGenerateOverlapEventsDuringLevelStreaming = true;

	if (BOOMPickUp != nullptr)
	{

	}
}

void ABOOMWeapon::Tick(float DeltaSeconds)
{

}

void ABOOMWeapon::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.4F, FColor::Cyan, "ABOOMWeapon::Fire()");


	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());

	if (PlayerController)
	{
		FRotator CameraRotation;
		FVector CameraLocation;
		CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

		FVector Start = CameraLocation;
		FVector End = Start + (CameraRotation.Vector() * HitscanRange);
		FHitResult HitResult;
		FCollisionQueryParams TraceParams;

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
		if (bHit)
		{
			UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponDamage, Character->GetController(), Character, DamageType);
		}

	}
	
}

void ABOOMWeapon::HandleReloadInput()
{
	if (CurrentState != nullptr)
	{
		CurrentState->HandleReloadInput();
	}
}

void ABOOMWeapon::ReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0F, FColor::Cyan, "ABOOMWeapon::Reload() occurred");

	//if we have Reserve Ammo to reload the weapon, then we can do this
	if (CurrentAmmoReserves > 0)
	{
		//How much ammo is missing from the mags ammo capacity
		int BulletDifference = (MagazineSize - CurrentAmmo);

		if (CurrentAmmoReserves >= MagazineSize)
		{
			CurrentAmmo += BulletDifference;
			CurrentAmmoReserves -= BulletDifference;
		}
		else
		{
			CurrentAmmo += CurrentAmmoReserves;
			CurrentAmmoReserves = 0;
		}
	}
	GotoState(ActiveState);
}

void ABOOMWeapon::Interact()
{

}

void ABOOMWeapon::Interact(ABOOMCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Red, FString::FromInt(Character->Weapons.Num()));
	if (Character == nullptr  || Character->Weapons.Num() >= Character->MaxWeaponsEquipped)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Red, "idk why");

		return;
	}
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

	Character->Weapons.Add(this);

	if ( Character->Weapons.Num() == 1)
	{
		//GotoState(ActiveState);
		GotoState(EquippingState);
		AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	}
	else
	{
		AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("spine_01")));

		GotoState(InactiveState);
	}
	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Character->SetHasRifle(true);
 

}

void ABOOMWeapon::OnInteractionRangeEntered(ABOOMCharacter* TargetCharacter)
{
	const  FString cont(TEXT("cont"));

	check(TargetCharacter)
	FItemInformation* WeaponData = TargetCharacter->WeaponTable->FindRow<FItemInformation>(Name, cont, true);

	if (WeaponData)
	{

		check(TargetCharacter->PlayerHUD)
		//@todo put all into one function
		TargetCharacter->GetPlayerHUD()->PickUpPrompt->SetPromptImage(WeaponData->ItemImage);
		TargetCharacter->GetPlayerHUD()->PickUpPrompt->SetPromptText(Name);
		TargetCharacter->GetPlayerHUD()->PickUpPrompt->SetVisibility(ESlateVisibility::Visible);

	}
}

void ABOOMWeapon::OnInteractionRangeExited(ABOOMCharacter* TargetCharacter)
{
	check(TargetCharacter->GetPlayerHUD())
	TargetCharacter->GetPlayerHUD()->PickUpPrompt->SetVisibility(ESlateVisibility::Hidden);
}

void ABOOMWeapon::HandleFireInput()
{
	if (CurrentState != nullptr)
	{
		CurrentState->HandleFireInput();
	}
}

void ABOOMWeapon::HandleEquipping()
{
	CurrentState->HandleEquipping();
}

void ABOOMWeapon::HandleUnequipping()
{
	CurrentState->HandleUnequipping();
}

void ABOOMWeapon::GotoState(UBOOMWeaponState* NewState)
{
	//CurrentState = NewState;
	/*Testing stuff in here while I figure out to handle weapon reference.*/
	/*Must be careful to avoid weapon glitches when the characcter code does something and the player code does as well.*/

	if (CurrentState != nullptr)
	{
		CurrentState = NewState;

		CurrentState->EnterState();
	}
}