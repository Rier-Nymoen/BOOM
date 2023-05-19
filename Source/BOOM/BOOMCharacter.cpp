// Copyright Epic Games, Inc. All Rights Reserved.

#include "BOOMCharacter.h"
#include "BOOMProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Weapons/BOOMWeapon.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/InteractableInterface.h"
#include "UI/BOOMPlayerHUD.h"
#include "Containers/Array.h"
#include "Math/NumericLimits.h"


//////////////////////////////////////////////////////////////////////////
// ABOOMCharacter

ABOOMCharacter::ABOOMCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Character doesnt have a rifle at start
	bHasRifle = false;
	bIsOverlappingWeapon = false;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	
	InteractionRange = 250.0F;
}

void ABOOMCharacter::BeginPlay()
{
	Super::BeginPlay();

	//possibly rework
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABOOMCharacter::OnCharacterBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABOOMCharacter::OnCharacterEndOverlap);

	//For Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		PlayerHUD = CreateWidget<UBOOMPlayerHUD>(PlayerController, PlayerHUDClass);
		check(PlayerHUD);
		PlayerHUD->AddToPlayerScreen();
	}
}

void ABOOMCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Setup Playcontroller


	/*
	* Player-look functionality for interacting with objects. Look interactivity is prioritized over proximity pickup.
	*/
	APlayerController* PlayerController = Cast<APlayerController>(this->GetController());
	if (PlayerController)
	{
		FRotator CameraRotation;
		FVector CameraLocation;
		CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

		FVector Start = CameraLocation;
		FVector End = Start + (CameraRotation.Vector() * InteractionRange);
		FHitResult HitResult;
		FCollisionQueryParams TraceParams;

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
		DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 0.1f);

		IInteractableInterface* InteractableObject;
		if (bHit)
		{
			if(HighlightedActor == HitResult.GetActor())
			{
				return;
			}
			
			HighlightedActor = HitResult.GetActor();

			InteractableObject = Cast<IInteractableInterface>(HighlightedActor);
			if (InteractableObject)
			{
				InteractableObject->OnInteractionRangeEntered(this);
			}
		}
		else
		{
			InteractableObject = Cast<IInteractableInterface>(HighlightedActor);
			HighlightedActor = nullptr;
			if (InteractableObject)
			{
				InteractableObject->OnInteractionRangeExited(this);
			}

			if (OverlappedActors.Num() > 0)
			{
				this->GetNearestInteractable(); 
			}

		}
	}
}

void ABOOMCharacter::OnCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	this->GetNearestInteractable();

}	


void ABOOMCharacter::OnCharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	this->GetNearestInteractable();

}

//////////////////////////////////////////////////////////////////////////// Input
void ABOOMCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABOOMCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABOOMCharacter::Look);

		//Cyclical Weapon Swapping
		EnhancedInputComponent->BindAction(WeaponSwapAction, ETriggerEvent::Triggered, this, &ABOOMCharacter::SwapWeapon);

		//Weapon Pickup
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ABOOMCharacter::Interact);
		//Fire Weapon
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABOOMCharacter::Fire);

	}
}

UBOOMPlayerHUD* ABOOMCharacter::GetPlayerHUD()
{
	return PlayerHUD;
}


//getters and setters because I plan on using them to track information or update HUD images.
void ABOOMCharacter::SetCurrentWeapon(ABOOMWeapon* Weapon)
{
	CurrentWeapon = Weapon;
}

ABOOMWeapon* ABOOMCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

ABOOMWeapon* ABOOMCharacter::GetPrimaryWeapon()
{
	return PrimaryWeapon;
}

ABOOMWeapon* ABOOMCharacter::GetSecondaryWeapon()
{
	return SecondaryWeapon;
}

void ABOOMCharacter::SetPrimaryWeapon(ABOOMWeapon* Weapon)
{
	PrimaryWeapon = Weapon;
	return;
}

void ABOOMCharacter::SetSecondaryWeapon(ABOOMWeapon* Weapon)
{
	SecondaryWeapon = Weapon;
	return;
}

void ABOOMCharacter::EquipWeapon(ABOOMWeapon* Weapon)
{
		if(GetPrimaryWeapon() == nullptr)
		{
			SetPrimaryWeapon(Weapon);
		}
		else if (GetSecondaryWeapon() == nullptr)
		{
			SetSecondaryWeapon(Weapon);
		}

		
		SetCurrentWeapon(Weapon);

}

void ABOOMCharacter::GetNearestInteractable()
{
	float MinDistance = TNumericLimits<float>::Max();

	GetOverlappingActors(OverlappedActors);
	for (AActor* OverlappedActor : OverlappedActors)
	{
		IInteractableInterface* InteractableObject = Cast<IInteractableInterface>(OverlappedActor);

		if (InteractableObject)
		{
			float DistanceToPlayer = FVector::Distance(this->GetActorLocation(), OverlappedActor->GetActorLocation());
			if (DistanceToPlayer <= MinDistance)
			{
				MinDistance = DistanceToPlayer;
				HighlightedActor = OverlappedActor;
			}
		}
	}
	if (IInteractableInterface* InteractableObject = Cast<IInteractableInterface>(HighlightedActor))
	{
		InteractableObject->OnInteractionRangeEntered(this);
	}
}



void ABOOMCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ABOOMCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABOOMCharacter::SwapWeapon(const FInputActionValue& Value)
{
	if (GetCurrentWeapon() == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0F, FColor::Green, "No Weapon Equipped");
		return;
	}
	//play Weaponswap Animation or replay pickup animation?


	//assumes if you have a current weapon, you always have a primary weapon.


	if (GetCurrentWeapon() == GetPrimaryWeapon())
	{
		SetCurrentWeapon(SecondaryWeapon);
	}
	else
	{
		SetCurrentWeapon(PrimaryWeapon);
	}
}

void ABOOMCharacter::Fire(const FInputActionValue& Value)
{

	if (GetCurrentWeapon())
	{
		GetCurrentWeapon()->Fire();
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.0F, FColor::Cyan, "null");
		//FRotator CameraRotation;
		//FVector CameraLocation;
		//APlayerController* PlayerController = Cast<APlayerController>(this->GetController());
		//check(PlayerController)
		//CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		//CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		//FVector Start = CameraLocation;
		//FVector End = Start + (CameraRotation.Vector() * InteractionRange);
		//DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, true, 4.0f);

	}
}

void ABOOMCharacter::Interact(const FInputActionValue& Value)
{

	//3/31/23 - WEAPON INTERACTION system looks great right now
	//@TODO fix issue where overlapped weapon ui wont show up after weapon has been picked up.
	//@TODO have nearest overlapped actor be the weapon which gets picked up. Linear scan.

	IInteractableInterface* InteractableObject;
	if (HighlightedActor)
	{
		InteractableObject = Cast<IInteractableInterface>(HighlightedActor);
		if(InteractableObject)
		{
			InteractableObject->Interact(this);
			InteractableObject->OnInteractionRangeExited(this);
			
		}
		this->GetNearestInteractable();
		return;

	}

	GetOverlappingActors(OverlappedActors);
	for (AActor* OverlappedActor : OverlappedActors)
	{
		InteractableObject = Cast<IInteractableInterface>(OverlappedActor);
		if (InteractableObject)
		{
			InteractableObject->Interact(this);

			break;
		}
	}

}

void ABOOMCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ABOOMCharacter::GetHasRifle()
{
	return bHasRifle;
}

