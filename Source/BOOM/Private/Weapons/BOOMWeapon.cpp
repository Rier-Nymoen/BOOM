// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeapon.h"
#include "BOOMPickUpComponent.h"
#include "BOOM/BOOMCharacter.h"
#include "UI/BOOMPlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Weapons/BOOMWeaponStateActive.h"
#include "Weapons/BOOMWeaponStateInactive.h"
#include "Weapons/BOOMWeaponStateEquipping.h"
#include "Weapons/BOOMWeaponStateFiring.h"



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
	UBOOMWeaponState* test = ActiveState;
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
	if (bCancelTimer)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_TestStateCanceling);
	}
}

void ABOOMWeapon::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.4F, FColor::Cyan, "ABOOMWeapon::Fire()");

	bCancelTimer = true;

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

void ABOOMWeapon::ReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.4F, FColor::Cyan, "ABOOMWeapon::Reload()");

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
		//bCanFire = true;
	}
	//bIsReloading = false;
}

void ABOOMWeapon::Interact()
{

}

void ABOOMWeapon::Interact(ABOOMCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr  || Character->Weapon != nullptr)
	{
		return;
	}

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	Character->Weapon = this;
	CurrentState = ActiveState;
	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	Character->SetHasRifle(true);

	//if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	//{
	//	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	//	{
	//		Subsystem->AddMappingContext(WeaponMappingContext, 1);
	//	}

	//	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	//	{
	//		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABOOMWeapon::Fire);
	//		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ABOOMWeapon::TimerStart);

	//	}

	//}

	//




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

void ABOOMWeapon::HandleFireInput()
{
	if (CurrentState != nullptr)
	{
		CurrentState->HandleFireInput();
	}
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

void ABOOMWeapon::TimerStart()
{
	bCancelTimer = false;
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.01F, FColor::Green, "TimerPressed");

	GetWorldTimerManager().SetTimer(TimerHandle_TestStateCanceling,this,  &ABOOMWeapon::TimerCall, 2.0f, false);

}

void ABOOMWeapon::TimerCall()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Cyan, "TimerCall()");
}
