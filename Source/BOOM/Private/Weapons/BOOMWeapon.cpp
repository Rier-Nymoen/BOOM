// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeapon.h"
#include "BOOMPickUpComponent.h"
#include "BOOM/BOOMCharacter.h"
#include "UI/BOOMPlayerHUD.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Weapons/BOOMWeaponStateActive.h"
#include "Weapons/BOOMWeaponStateInactive.h"
#include "Weapons/BOOMWeaponStateEquipping.h"
#include "Weapons/BOOMWeaponStateFiring.h"
#include "Weapons/BOOMWeaponStateReloading.h"
#include "Weapons/BOOMWeaponStateUnequipping.h"
#include "Math/UnrealMathUtility.h"
#include "AI/BOOMAIController.h"
#include "Camera/CameraComponent.h"


//@TODO decide if I am changing variable names

// Sets default values
ABOOMWeapon::ABOOMWeapon()
{
	//dont forget to turn off
	PrimaryActorTick.bCanEverTick = true;

	Weapon1P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh1P");
	
	BOOMPickUp = CreateDefaultSubobject<UBOOMPickUpComponent>("PickUpComponent");
	BOOMPickUp->SetupAttachment(Weapon1P);
	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

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
	FireRateSeconds = 0.066F; //@Todo configure to rpm

	//@TODO make based off firemodes
	WeaponDamage = 100.0F;

	CurrentState = InactiveState;
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


}


void ABOOMWeapon::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}


	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());

	//@TODO: revamp
	if (PlayerController)
	{
		FRotator CameraRotation;
		FVector CameraLocation;

		//Eventually must change these to adjust for aim based on weapon spread. 
		CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

		FVector Start = CameraLocation;

		FVector End = Start + (CalculateSpread(CameraRotation).Vector() * HitscanRange);

		FHitResult HitResult;
		FCollisionQueryParams TraceParams;

		//@TODO - think about a way to have modifiable ammo costs i.e. higher costs on a charged shot. Need charged shot,effects, etc to take place.
		AddAmmo(-AmmoCost);
		
		//Update the time weapon was fired.
		LastTimeFiredSeconds = GetWorld()->GetTimeSeconds();

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
		DrawDebugLine(GetWorld(), Start, End, FColor(FMath::FRandRange(0.0F,255.0F), FMath::FRandRange(0.0F, 255.0F), FMath::FRandRange(0.0F, 255.0F)), true, 0.4);

		if (Character->GetPlayerHUD())
		{
			Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);

		}

		if (bHit)
		{
			UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponDamage, Character->GetController(), Character, DamageType);
		}
		//return to avoid overnesting?
	}
	else
	{
		ABOOMAIController* AIController = Cast<ABOOMAIController>(Character->GetController());
		if (AIController)
		{
			//Placeholder code, just to test things.
			FVector Start  = GetActorLocation() + FVector(0,0,100);
			FVector End = Start + (CalculateSpread(GetActorRotation()).Vector() * HitscanRange);
			LastTimeFiredSeconds = GetWorld()->GetTimeSeconds();


			FHitResult HitResult;
			FCollisionQueryParams TraceParams;
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
			DrawDebugLine(GetWorld(), Start, End, FColor(FMath::FRandRange(0.0F, 255.0F), FMath::FRandRange(0.0F, 255.0F), FMath::FRandRange(0.0F, 255.0F)), true, 0.4);
			if (bHit)
			{
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponDamage, Character->GetController(), Character, DamageType);
			}
			//end of placeholder code
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
	if (CanReload()) //May be unnecessary but keeping here for now in case of unexpected issues.
	{								
		//How much ammo is missing from the mags ammo capacity
		int BulletDifference = (MagazineSize - CurrentAmmo);

		CurrentAmmo += FMath::Min(BulletDifference, CurrentAmmoReserves);
		CurrentAmmoReserves -= FMath::Min(BulletDifference, CurrentAmmoReserves);

		check(Character)
		if (Character->GetPlayerHUD())
		{
				Character->GetPlayerHUD()->GetWeaponInformationElement()->SetReserveAmmoText(CurrentAmmoReserves);
				Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);
		}


	}

	GotoState(ActiveState);

}


void ABOOMWeapon::Interact(ABOOMCharacter* TargetCharacter)
{
	Character = TargetCharacter;	
	if (Character == nullptr)
	{
		return;
	}
	Character->EquipWeapon(this);
}

void ABOOMWeapon::OnInteractionRangeEntered(ABOOMCharacter* TargetCharacter)
{
	const  FString cont(TEXT("cont"));

	check(TargetCharacter)
	FItemInformation* WeaponData = TargetCharacter->WeaponTable->FindRow<FItemInformation>(Name, cont, true);

	if (WeaponData)
	{

		if (TargetCharacter->GetPlayerHUD())
		{
			TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetPromptImage(WeaponData->ItemImage);
			TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetPromptText(Name);
			TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetVisibility(ESlateVisibility::Visible);
		}


	}
}

void ABOOMWeapon::OnInteractionRangeExited(ABOOMCharacter* TargetCharacter)
{
	if (TargetCharacter->GetPlayerHUD())
	{
		TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetVisibility(ESlateVisibility::Hidden);

	}
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
	CurrentAmmo = FMath::Clamp(CurrentAmmo + Amount, 0, MagazineSize);
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
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.F, FColor(255, 82, 255), "Start of ID: "+ GetName() + "Model Name:" + Name.ToString());

	CurrentState = NewState;
	CurrentState->EnterState();
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.F, FColor(255, 82, 255), "End of ID: " + GetName() + "Model Name:" + Name.ToString());


}

void ABOOMWeapon::GotoStateEquipping()
{
	GotoState(EquippingState);
}

void ABOOMWeapon::GotoStateActive()
{
	GotoState(ActiveState);
}

void ABOOMWeapon::GotoStateInactive()
{
	GotoState(InactiveState);
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
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.4F, FColor(69,2,180), "Ready to fire");

		return true;
	}
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.4F, FColor(180, 2, 69), "Not ready to fire");

	return false;
}

void ABOOMWeapon::HandleBeingDropped()
{
	FDetachmentTransformRules DetRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetRules);

	GotoState(UnequippingState);

	Weapon1P->SetSimulatePhysics(true);
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, false);
	BOOMPickUp->AttachToComponent(Weapon1P, AttachmentRules);
	if (Character)
	{
		//@TODO weapon could spawn or past objects - may solve issue a different way
	
		Weapon1P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		/*
		On character death, velocity will be 0, need to have character have velocity on death.
		or may need Velocity and forward vector recorded before death when dying
		*/
		Weapon1P->SetPhysicsLinearVelocity(Character->GetActorForwardVector() * Character->GetVelocity().Size(), true);
	}

	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Character = nullptr;

}

void ABOOMWeapon::DisableCollision()
{
	Weapon1P->SetSimulatePhysics(false);
	Weapon1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BOOMPickUp->SetSimulatePhysics(false);
	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

bool ABOOMWeapon::CanReload()
{
	return CurrentAmmoReserves > 0 && CurrentAmmo < MagazineSize;
}
