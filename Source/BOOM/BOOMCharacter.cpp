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
	
	InteractionRange = 200.0F;
}

void ABOOMCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABOOMCharacter::OnCharacterBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABOOMCharacter::OnCharacterEndOverlap);

	//Add Input Mapping Context
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

	//maybe i have a line and move it instead of redrawing it?
	APlayerController* PlayerController = Cast<APlayerController>(this->GetController());
	if (PlayerController)
	{
		FRotator CameraRotation;
		FVector CameraLocation;
		CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		//Learn how this is actually derived
		//outhit means its going to alter HitResult by reference, hence the Out naming  convention unreal uses
		FVector Start = CameraLocation;
		FVector End = Start + (CameraRotation.Vector() * InteractionRange);
		FHitResult HitResult;
		FCollisionQueryParams TraceParams;

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
		//DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, true, 4.0f);
		 nullptr;
		IInteractableInterface* InteractableObject;

		if (bHit)
		{
			//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0F, FColor::Blue, "Hit");
			//AActor* OtherActor = HitResult.GetActor();
			if(HighlightedActor == HitResult.GetActor())
			{
				return;
			}

			HighlightedActor = HitResult.GetActor();


			InteractableObject = Cast<IInteractableInterface>(HighlightedActor);
			if (InteractableObject)
			{

				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0F, FColor::Purple, "Looked at an interactable Object");

				InteractableObject->OnInteractionRangeEntered(this);
				
			}
		}
		else
		{
			InteractableObject = Cast<IInteractableInterface>(HighlightedActor);
			HighlightedActor = nullptr;
			if (InteractableObject)
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0F, FColor::Purple, "Stopped looking at object");

				InteractableObject->OnInteractionRangeExited(this);
			}
		}


	}
}


void ABOOMCharacter::OnCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractableInterface* InteractableObject;

	InteractableObject = Cast<IInteractableInterface>(OtherActor);
	if (InteractableObject)
	{
		InteractableObject->OnInteractionRangeEntered(this);
	}

}

void ABOOMCharacter::OnCharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractableInterface* InteractableObject;
	InteractableObject = Cast<IInteractableInterface>(OtherActor);
	if (InteractableObject)
	{
		InteractableObject->OnInteractionRangeExited(this);
	}
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

void ABOOMCharacter::SetCurrentWeapon(ABOOMWeapon* Weapon)
{
	CurrentWeapon = Weapon;
}

ABOOMWeapon* ABOOMCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
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

}

void ABOOMCharacter::Fire(const FInputActionValue& Value)
{

	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.0F, FColor::Cyan, "null");
		FRotator CameraRotation;
		FVector CameraLocation;
		APlayerController* PlayerController = Cast<APlayerController>(this->GetController());
		check(PlayerController)
		CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		FVector Start = CameraLocation;
		FVector End = Start + (CameraRotation.Vector() * InteractionRange);
		DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, true, 4.0f);

	}
}

void ABOOMCharacter::Interact(const FInputActionValue& Value)
{

	IInteractableInterface* InteractableObject;
	if (HighlightedActor)
	{
		InteractableObject = Cast<IInteractableInterface>(HighlightedActor);
		if(InteractableObject)
		{
			InteractableObject->Interact(this);
			InteractableObject->OnInteractionRangeExited(this);
		}
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

