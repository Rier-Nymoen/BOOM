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
#include "Math/UnrealMathUtility.h"



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
	//Player should spawn in with the max ammo reserves at the start of the game
	CurrentAmmoReserves = MaxAmmoReserves;

	AmmoCost = 1;

	MagazineSize = 10;

	//Player should spawn with a full magazine
	CurrentAmmo = MagazineSize;

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
	//bGenerateOverlapEventsDuringLevelStreaming = true;
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


	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());

	if (PlayerController)
	{
		FRotator CameraRotation;
		FVector CameraLocation;

		//Eventually must change these to adjust for aim based on weapon spread. 


		CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

		FVector Start = CameraLocation;
		FVector End = Start + (CameraRotation.Vector() * HitscanRange);
		FHitResult HitResult;
		FCollisionQueryParams TraceParams;

		//@TODO - think about a way to have modifiable ammo costs i.e. higher costs on a charged shot. Need charged shot,effects, etc to take place.
		AddAmmo(-AmmoCost);

		

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);

		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);

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

	//@TODO possible to fail if we have a person throws weapon, state doesnt cancel. but handle that in state.
	check(Character)
	Character->GetPlayerHUD()->GetWeaponInformationElement()->SetReserveAmmoText(CurrentAmmoReserves);
	Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);

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

	//FAttachmentTransformRules Att(EAttachmentRule::SnapToTarget, EDetachmentRule::KeepRelative, true);


	Character->Weapons.Add(this);
	//seems wonky that the weapon is handling logic for what the character is doing. Maybe find a way to move some of this onto the character.
	if ( Character->Weapons.Num() == 1)
	{
		GotoState(EquippingState);
		AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetWeaponNameText(Name);
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetReserveAmmoText(CurrentAmmoReserves);

	}
	else
	{
		GotoState(InactiveState);
		AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("spine_01")));

	

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
		TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetPromptImage(WeaponData->ItemImage);
		TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetPromptText(Name);
		TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetVisibility(ESlateVisibility::Visible);

	}
}

void ABOOMWeapon::OnInteractionRangeExited(ABOOMCharacter* TargetCharacter)
{
	check(TargetCharacter->GetPlayerHUD())
	TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetVisibility(ESlateVisibility::Hidden);
}

void ABOOMWeapon::HandleFireInput()
{
	//probably want to crash if this is null.
	CurrentState->HandleFireInput();
	
}

void ABOOMWeapon::HandleStopFireInput()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, FColor::Red, "erw");
	CurrentState->HandleStopFiringInput();
}

void ABOOMWeapon::AddAmmo(int Amount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo + Amount, 0, MaxAmmoReserves);
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

	if (NewState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to assign an invalid or null state."));
		return;
	}
	UBOOMWeaponState* PreviousState = CurrentState;

	if (CurrentState != nullptr)
	{
		PreviousState->ExitState();
	}
	CurrentState = NewState;
	CurrentState->EnterState();
	 

}

