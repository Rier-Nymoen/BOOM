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


//@TODO decide if I am changing variable names

// Sets default values
ABOOMWeapon::ABOOMWeapon()
{
	//dont forget to turn off
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

	MaxAmmoReserves = 210;
	//Player should spawn in with the max ammo reserves at the start of the game
	CurrentAmmoReserves = MaxAmmoReserves;


	AmmoCost = 1;

	MagazineSize = 30;

	//Player should spawn with a full magazine
	CurrentAmmo = MagazineSize;

	ReloadDurationSeconds = 2.2f;

	HitscanRange = 2000.0F;

	LastTimeFiredSeconds = -1.0F;

	//FireRateSeconds = 0.066F;
	FireRateSeconds = 0.18F;

	//@TODO change to hitscan damage, projectiles have their own damage stat.
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
}

void ABOOMWeapon::Tick(float DeltaSeconds)
{
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1000.0F, FColor(FMath::FRandRange(0.0F, 255.0F), FMath::FRandRange(0.0F, 255.0F), FMath::FRandRange(0.0F, 255.0F)), FString::SanitizeFloat(GetWorld()->GetTimeSeconds()));

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
	/*	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Magenta, FString(CameraLocation.ToString()));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Cyan, FString(CameraRotation.ToString()));*/

		FVector Start = CameraLocation;

		//NoSpreadCalculator
		//FVector End = Start + (CameraRotation.Vector() * HitscanRange);

		FVector End = Start + (CalculateSpread(CameraRotation).Vector() * HitscanRange);



		FHitResult HitResult;
		FCollisionQueryParams TraceParams;

		//@TODO - think about a way to have modifiable ammo costs i.e. higher costs on a charged shot. Need charged shot,effects, etc to take place.
		AddAmmo(-AmmoCost);
		
		//Update the time weapon was fired.
		LastTimeFiredSeconds = GetWorld()->GetTimeSeconds();

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
		DrawDebugLine(GetWorld(), Start, End, FColor(FMath::FRandRange(0.0F,255.0F), FMath::FRandRange(0.0F, 255.0F), FMath::FRandRange(0.0F, 255.0F)), true, 0.4);

		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);

		if (bHit)
		{
			UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponDamage, Character->GetController(), Character, DamageType);
		}

	}
	
}

bool ABOOMWeapon::IsIntendingToRefire()
{
	if (GetCharacter()->bIsPendingFiring && HasAmmo())
	{
		return true;
	}
	if (CurrentAmmo <= 0 && CurrentAmmoReserves > 0)
	{
		
		GotoState(ReloadingState);
	}
	else
	{
		GotoState(ActiveState);
	}
	return false;
}

ABOOMCharacter* ABOOMWeapon::GetCharacter()
{

	checkSlow(Character)
	return Character;
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
	{								// will need to check bullet difference != 0 maybe
		//How much ammo is missing from the mags ammo capacity
		int BulletDifference = (MagazineSize - CurrentAmmo);

		CurrentAmmo += FMath::Min(BulletDifference, CurrentAmmoReserves);
		CurrentAmmoReserves -= FMath::Min(BulletDifference, CurrentAmmoReserves);

		check(Character)
			Character->GetPlayerHUD()->GetWeaponInformationElement()->SetReserveAmmoText(CurrentAmmoReserves);
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);

		GotoState(ActiveState);
	}
}


void ABOOMWeapon::Interact(ABOOMCharacter* TargetCharacter)
{
	Character = TargetCharacter;	
	if (Character == nullptr)
	{
		return;
	}
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

	//seems wonky that the weapon is handling logic for what the character is doing. Could have this function call the character's equip function. However, unsure.
	if ( Character->Weapons.Num() == 0)
	{
		GotoState(EquippingState);
		AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetWeaponNameText(Name);
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetReserveAmmoText(CurrentAmmoReserves);
		Character->Weapons.Add(this); //using add will not work here.

	}
	else if(Character->Weapons.Num() != Character->MaxWeaponsEquipped)
	{

		GotoState(InactiveState);
		AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("spine_01")));
		Character->Weapons.Add(this); //using add will not work here.

	}
	else
	{
		Character->DropCurrentWeapon();
		Character->Weapons[Character->CurrentWeaponSlot] = this;
		GotoState(EquippingState);
		AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetWeaponNameText(Name);
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetReserveAmmoText(CurrentAmmoReserves);
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
	CurrentState->HandleFireInput();
}

void ABOOMWeapon::HandleStopFireInput()
{
	CurrentState->HandleStopFiringInput();
}

void ABOOMWeapon::AddAmmo(int Amount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo + Amount, 0, MaxAmmoReserves);
}

bool ABOOMWeapon::HasAmmo()
{
	if (CurrentAmmo >= AmmoCost)
	{
		return true;
	}
	else
	{
		return false;
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

FRotator ABOOMWeapon::CalculateSpread(FRotator PlayerLookRotation)
{
	float MaxSpreadX = 5.0F;
	float MaxSpreadZ = 5.0F;

	float MinSpreadX = 0.3F;
	float MinSpreadZ = 0.3F;

	float AdjustedSpreadX = FMath::RandRange(MinSpreadX, MaxSpreadX);
	float AdjustedSpreadZ = FMath::RandRange(MinSpreadZ, MaxSpreadZ);

	AdjustedSpreadX = FMath::RandRange(-AdjustedSpreadX, AdjustedSpreadX);
	AdjustedSpreadZ = FMath::RandRange(-AdjustedSpreadZ, AdjustedSpreadZ);

	FRotator Offset(AdjustedSpreadX, AdjustedSpreadZ, 0);

	return (PlayerLookRotation + Offset);





	//account for normal rotation and position, offset rounds somewhat based on ymin ymax
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

float ABOOMWeapon::GetFireRateSeconds()
{
	return FireRateSeconds;
}


//move functionality
float ABOOMWeapon::GetLastTimeFiredSeconds()
{
	return LastTimeFiredSeconds;
}

bool ABOOMWeapon::IsReadyToFire()
{
	if ((GetWorld()->GetTimeSeconds() - LastTimeFiredSeconds  ) >= ( FireRateSeconds ))
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1000.0F, FColor(69,2,180), "Ready to fire");

		return true;
	}
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1000.0F, FColor(180, 2, 69), "Not ready to fire");

	return false;
}

void ABOOMWeapon::HandleBeingDropped()
{
	FDetachmentTransformRules DetRules(EDetachmentRule::KeepWorld, true);

	this->GotoState(InactiveState);
	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Character = nullptr;
	DetachFromActor(DetRules);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor(20, 69, 103), "handlebeingdropped");

}
