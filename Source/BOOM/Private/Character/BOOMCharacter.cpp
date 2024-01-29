//too many headers
#include "Character/BOOMCharacter.h"
#include "BOOM/BOOMProjectile.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Weapons/BOOMWeapon.h"
#include "Weapons/BOOMGrenade.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/InteractableInterface.h"
#include "UI/BOOMPlayerHUD.h"
#include "Containers/Array.h"
#include "Math/NumericLimits.h"
#include "Character/BOOMHealthComponent.h"
#include "Character/BOOMCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Character/BOOMAttributeSetBase.h"
#include "Kismet/GameplayStatics.h"
#include "BOOMGameplayAbility.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
//////////////////////////////////////////////////////////////////////////
// ABOOMCharacter

ABOOMCharacter::ABOOMCharacter()
{	
	PrimaryActorTick.bCanEverTick = true;
	// Character doesnt have a rifle at start
	bHasRifle = false;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;
	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("CharacterMesh1P");
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(CameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;

	GetMesh()->SetOnlyOwnerSee(true);
	//Debating on own 3P mesh, or just using the mesh class as the 3p mesh.
	//Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("CharacterMesh3P");
	//Mesh3P->SetOnlyOwnerSee(false);
	//Mesh3P->SetupAttachment(FirstPersonCameraComponent);
	//Mesh3P->bCastDynamicShadow = true;
	//Mesh3P->CastShadow = true;

	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	SocketNameGripPoint = "GripPoint";
	SocketNameHolsterPoint = "spine_01";
	SocketNameGripPoint3P = "hand_r";
	InteractionRange = 250.0F;
	Overlaps = 0;
	bGenerateOverlapEventsDuringLevelStreaming = true;
	CurrentWeaponSlot = 0;
	MaxWeaponsEquipped = 2;
	bIsPendingFiring = false;

	PlayerHUD = nullptr;

	bIsFocalLengthScalingEnabled = false;

	////may have to use FObjectInitializer, will see.
	BOOMCharacterMovementComponent = Cast<UBOOMCharacterMovementComponent>(GetCharacterMovement());

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSetBase = CreateDefaultSubobject<UBOOMAttributeSetBase>("AttributeSetBase"); /*There is a known bug (not on my end) with child blueprints and attribute sets*/
	AttributeSetBase->InitHealth(100.F);
	AttributeSetBase->InitMaxHealth(100.F);
	AttributeSetBase->InitShieldStrength(100.F);
	AttributeSetBase->InitMaxShieldStrength(100.F);

	//Can always get handles to delegates if needed.

	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ABOOMCharacter::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetShieldStrengthAttribute()).AddUObject(this, &ABOOMCharacter::HandleShieldStrengthChanged);
	ShieldRechargeInterpSeconds = 0.01f;
	ShieldFullRechargeDurationSeconds = 3.f;
	ShieldRechargeDelaySeconds = 5.F;

	HealthComponent = CreateDefaultSubobject<UBOOMHealthComponent>("HealthComponent");
	HealthComponent->InitializeComponentWithOwningActor(AbilitySystemComponent);
}

void ABOOMCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABOOMCharacter::OnCharacterBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABOOMCharacter::OnCharacterEndOverlap);
	OnDeath.AddDynamic(this, &ABOOMCharacter::HandleDeath);

	//check(AttributeSetBase) //sanity check cause of potential corrupted asset bug
	if (AttributeSetBase)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttributeSetBase is set"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AttributeSetBase is not set"))
	}
	//}

	HealthComponent->OnHealthChanged.AddDynamic(this, &ABOOMCharacter::HandleHealthChanged);
	/*
	Actors already overlapping will not cause a begin overlap event, therefore need to check size of overlapped actors on begin play.
	*/
	GetOverlappingActors(OverlappedActors);
	Overlaps = OverlappedActors.Num();
	
	//For Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		if (PlayerHUDClass)
		{
			PlayerHUD = CreateWidget<UBOOMPlayerHUD>(PlayerController, PlayerHUDClass);

			if (PlayerHUD)
			{
				PlayerHUD->AddToPlayerScreen();
			}
		}

		FTimerHandle InteractionHandle;

		GetWorld()->GetTimerManager().SetTimer(InteractionHandle, this, &ABOOMCharacter::CheckPlayerLook, 0.1F, true);
	}
	SpawnWeapons();

}

void ABOOMCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}

void ABOOMCharacter::OnCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Overlaps++;
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Red, FString::FromInt(Overlaps));

}	


void ABOOMCharacter::OnCharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Overlaps--;
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Blue, FString::FromInt(Overlaps));

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

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABOOMCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ABOOMCharacter::EndCrouch); 

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABOOMCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABOOMCharacter::Look);

		//Cyclical Weapon Swapping
		EnhancedInputComponent->BindAction(WeaponSwapAction, ETriggerEvent::Started, this, &ABOOMCharacter::SwapWeapon);

		//Weapon Pickup
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ABOOMCharacter::Interact);

		//Fire Weapon
		//@TODO - Possibly let weapons bind their own firing input responses
		//EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABOOMCharacter::StartFire);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Started, this, &ABOOMCharacter::Zoom);

		//EnhancedInputComponent->BindAction(StopFireAction, ETriggerEvent::Completed, this, &ABOOMCharacter::StopFire);

		//Reload Weapon
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ABOOMCharacter::Reload);

		EnhancedInputComponent->BindAction(GrenadeThrowAction, ETriggerEvent::Started, this, &ABOOMCharacter::ThrowGrenade);

	}
}

void ABOOMCharacter::Reload()
{
	if (HasNoWeapons())
	{
		return;
	}
	if (Weapons.IsValidIndex(CurrentWeaponSlot))
	{
		Weapon = Weapons[CurrentWeaponSlot];
	}
	if (Weapon != nullptr)
	{
		Weapon->HandleReloadInput();
	}
}

void ABOOMCharacter::Fire()
{

}

void ABOOMCharacter::StopFire()
{
	bIsPendingFiring = false;
	if (Weapons.IsValidIndex(CurrentWeaponSlot))
	{
		if (Weapons[CurrentWeaponSlot] != nullptr)
		{
			Weapons[CurrentWeaponSlot]->HandleStopFireInput();
		}
	}
}

void ABOOMCharacter::DropCurrentWeapon()
{
	if (Weapons.IsValidIndex(CurrentWeaponSlot))
	{
		if (Weapons[CurrentWeaponSlot] != nullptr)
		{
			Weapons[CurrentWeaponSlot]->HandleBeingDropped();
		}
	}
}

UBOOMPlayerHUD* ABOOMCharacter::GetPlayerHUD()
{
	return PlayerHUD;
}


AActor* ABOOMCharacter::GetNearestInteractable()
{
	float MinDistance = TNumericLimits<float>::Max();
	AActor* NearestActor = nullptr;
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
				NearestActor = OverlappedActor;
			}
		}
	}
	return NearestActor;
}


void ABOOMCharacter::CheckPlayerLook()
{
	/*
	Player-look functionality for interacting with objects. Look interactivity is prioritized over proximity pickup.
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

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel4, TraceParams);
		
		IInteractableInterface* InteractableObject;
		if (bHit)
		{
			if (HighlightedActor == HitResult.GetActor())
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
			
			if(Overlaps > 0)
			{
				HighlightedActor = this->GetNearestInteractable();
				if (HighlightedActor)
				{
					InteractableObject = Cast<IInteractableInterface>(HighlightedActor);

					InteractableObject->OnInteractionRangeEntered(this);
				}			
			}
		}
	}
}

//@TODO: Revamp later when other inventory features are implemented.
void ABOOMCharacter::SpawnWeapons()
{
	if (WeaponsToSpawn.Num() > MaxWeaponsEquipped)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0F, FColor::Red, "Too many weapons added to character");
	}
	
	for (TSubclassOf<ABOOMWeapon> WeaponClass : WeaponsToSpawn)
	{
		if(!WeaponClass)
		{
			continue;
		}

		ABOOMWeapon* SpawnedWeapon = GetWorld()->SpawnActor<ABOOMWeapon>(WeaponClass, GetActorLocation(), GetActorRotation());
		check(SpawnedWeapon)
		if (Weapons.Num() > MaxWeaponsEquipped)
		{
			break;
		}
		EquipWeapon(SpawnedWeapon);
	}

}

void ABOOMCharacter::ThrowGrenade()
{
	////plays animation for throwing and spawns the thrown projectile. That should be about it.
	//ABOOMGrenade* SpawnedGrenade = Cast<ABOOMGrenade>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, GrenadeType, CameraComponent->GetComponentTransform()));
	///*
	//What's a better way I can accomplish this?
	//*/
	//if (SpawnedGrenade)
	//{
	//	SpawnedGrenade->SetInstigator(this);
	//	GetCapsuleComponent()->MoveIgnoreActors.Add(SpawnedGrenade);
	//	//still need to add velocity of moving person?
	//	FVector ThrowDirection = CameraComponent->GetForwardVector();
	//	SpawnedGrenade->ApplyThrownVelocity(ThrowDirection);
	//	//SpawnedGrenade->GetCollisionComp()->MoveIgnoreActors.Add(this);
	//	UGameplayStatics::FinishSpawningActor(SpawnedGrenade, CameraComponent->GetComponentTransform());
	//}
	if (BOOMCharacterMovementComponent)
	{
		BOOMCharacterMovementComponent->DetectMantleableSurface();

	}


}

void ABOOMCharacter::EquipWeapon(ABOOMWeapon* TargetWeapon)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	TargetWeapon->Character = this;
	TargetWeapon->SetOwner(this);

	TargetWeapon->DisableCollision();
	if (HasNoWeapons())
	{
		Weapons.Add(TargetWeapon);


		TargetWeapon->GotoStateEquipping();

		//jank placeholder
		if (GetMesh1P())
		{
			TargetWeapon->AttachToComponent(GetMesh1P(), AttachmentRules, SocketNameGripPoint);
		}
		if (TargetWeapon->GetMeshWeapon3P() && GetMesh())
		{
			TargetWeapon->GetMeshWeapon3P()->AttachToComponent(GetMesh(), AttachmentRules, SocketNameGripPoint3P);
		}

		
		if (PlayerHUD)
		{
			GetPlayerHUD()->GetWeaponInformationElement()->SetWeaponNameText(Weapons[CurrentWeaponSlot]->Name);
			GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(Weapons[CurrentWeaponSlot]->CurrentAmmo);
			GetPlayerHUD()->GetWeaponInformationElement()->SetReserveAmmoText(Weapons[CurrentWeaponSlot]->CurrentAmmoReserves);
		}

	}
	else if (HasEmptyWeaponSlots())
	{
		Weapons.Add(TargetWeapon);

		TargetWeapon->GotoStateInactive();
		TargetWeapon->AttachToComponent(GetMesh1P(), AttachmentRules, SocketNameHolsterPoint);

	}
	else
	{
		/*
		@TODO:  Avoid or Prevent - If collision is enabled, the character can pick up a weapon that is in their hand and also attempt to drop it, which causes the
		weapon's character reference to be assigned to the same character, and then nullified, causing seg faults.
		*/
		DropCurrentWeapon();
		TargetWeapon->GotoStateEquipping();
		Weapons[CurrentWeaponSlot] = TargetWeapon;
		TargetWeapon->AttachToComponent(GetMesh1P(), AttachmentRules, SocketNameGripPoint);
		if (TargetWeapon->GetMeshWeapon3P() && GetMesh())
		{
			TargetWeapon->GetMeshWeapon3P()->AttachToComponent(GetMesh(), AttachmentRules, SocketNameGripPoint3P);
		}

		if (PlayerHUD)
		{
			GetPlayerHUD()->GetWeaponInformationElement()->SetWeaponNameText(Weapons[CurrentWeaponSlot]->Name);
			GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(Weapons[CurrentWeaponSlot]->CurrentAmmo);
			GetPlayerHUD()->GetWeaponInformationElement()->SetReserveAmmoText(Weapons[CurrentWeaponSlot]->CurrentAmmoReserves);
		}
	}
	SetHasRifle(true);
}

bool ABOOMCharacter::HasNoWeapons()
{
	return Weapons.Num() == 0;
}

bool ABOOMCharacter::HasEmptyWeaponSlots()
{
	return Weapons.Num() != MaxWeaponsEquipped;
}

void ABOOMCharacter::HandleDeath()
{
	if (Controller)
	{
		Controller->UnPossess();
	}
	//todo - specify function
	//otherwise could cause bug hard to find

	ThrowInventory();

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRechargeDelay);

	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromParent();
	}
	if (DeathMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Montage Executed"))
		PlayAnimMontage(DeathMontage);
	}
}

void ABOOMCharacter::ThrowInventory()
{
	for (ABOOMWeapon* TWeapon : Weapons) //jank code 
	{
		TWeapon->HandleBeingDropped();
		//Can isolate death physics behaviors or have dropping behavior that is shared on death.
	}
	Weapons.Empty();
}

void ABOOMCharacter::InterpShieldRegen()
{
	//UE_LOG(LogTemp, Warning, TEXT("RegenInterval"))

	float ShieldStrengthFloat = AttributeSetBase->GetShieldStrength();

	float ShieldMax = AttributeSetBase->GetMaxShieldStrength();
	//UE_LOG(LogTemp, Warning, TEXT("Shield Max: %f"), ShieldMax)

	

	float UpdateVal = ShieldStrengthFloat + ((ShieldMax /ShieldFullRechargeDurationSeconds) * ShieldRechargeInterpSeconds);

	//UE_LOG(LogTemp, Warning, TEXT("Update Val: %f"), UpdateVal)

	AttributeSetBase->SetShieldStrength(UpdateVal);
}


void ABOOMCharacter::RegenerateShields()
{
	//local for testing
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShieldRecharge, this, &ABOOMCharacter::InterpShieldRegen, ShieldRechargeInterpSeconds, true);
}

void ABOOMCharacter::HandleShieldStrengthChanged(const FOnAttributeChangeData& Data)
{
 
	if (FMath::IsNearlyEqual(Data.OldValue, Data.NewValue))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecharge);
	}

	if (Data.OldValue > Data.NewValue)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecharge);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShieldRechargeDelay, this, &ABOOMCharacter::RegenerateShields, ShieldRechargeDelaySeconds);
	}
	
	if (PlayerHUD)
	{
		if (AttributeSetBase)
		{
			PlayerHUD->GetHealthInformationElement()->SetShieldBar(Data.NewValue / AttributeSetBase->GetMaxShieldStrength());
		}

		//move to shield component
	}

}

void ABOOMCharacter::SetupCharacterAbilities()
{
	for (TSubclassOf<UBOOMGameplayAbility>& InitialAbility: CharacterAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(InitialAbility, 1, 1, this);

		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}


void ABOOMCharacter::HandleHealthChanged(float OldValue, float NewValue)
{
	check(HealthComponent)


	if (OldValue > NewValue)
	{

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecharge);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShieldRechargeDelay, this, &ABOOMCharacter::RegenerateShields, ShieldRechargeDelaySeconds);
	}

	if (PlayerHUD)
	{

		if (AttributeSetBase)
		{
			PlayerHUD->GetHealthInformationElement()->SetHealthBar(NewValue / AttributeSetBase->GetMaxHealth());
		}

	}

	if (!IsAlive())
	{
		OnDeath.Broadcast();
		return;
	}
}

void ABOOMCharacter::Zoom()
{
	if (Weapons.IsValidIndex(CurrentWeaponSlot) && Weapons[CurrentWeaponSlot] != nullptr)
	{
		Weapons[CurrentWeaponSlot]->Zoom();
	}
}

float ABOOMCharacter::GetHealth()
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetHealth();
	}
	
	return 0.0F;
}

bool ABOOMCharacter::IsAlive()
{
	return GetHealth() > 0.0F;
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
		//@TODO: Move to player settings and include save system

		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

	}
}

void ABOOMCharacter::SwapWeapon(const FInputActionValue& Value)
{
	if (Weapons.Num() <= 1)
	{
		return;
	}
	if (Weapons.IsValidIndex(CurrentWeaponSlot) && Weapons[CurrentWeaponSlot] != nullptr)
	{

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
		
		Weapons[CurrentWeaponSlot]->AttachToComponent(this->GetMesh1P(), AttachmentRules, SocketNameHolsterPoint);
		
		Weapons[CurrentWeaponSlot]->HandleUnequipping();

		//
		CurrentWeaponSlot++;
		CurrentWeaponSlot = (CurrentWeaponSlot % MaxWeaponsEquipped);

		if (Weapons.IsValidIndex(CurrentWeaponSlot) && Weapons[CurrentWeaponSlot] != nullptr) //maybe use isValidIndex instead
		{
			Weapons[CurrentWeaponSlot]->AttachToComponent(this->GetMesh1P(), AttachmentRules, SocketNameGripPoint);
			Weapons[CurrentWeaponSlot]->HandleEquipping();
			if (PlayerHUD)
			{
				GetPlayerHUD()->GetWeaponInformationElement()->SetWeaponNameText(Weapons[CurrentWeaponSlot]->Name);
				GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(Weapons[CurrentWeaponSlot]->CurrentAmmo);
				GetPlayerHUD()->GetWeaponInformationElement()->SetReserveAmmoText(Weapons[CurrentWeaponSlot]->CurrentAmmoReserves);
			}
		}
	}
}

void ABOOMCharacter::StartCrouch(const FInputActionValue& Value)
{
	Crouch();
}

void ABOOMCharacter::EndCrouch(const FInputActionValue& Value)
{
	UnCrouch();
}

void ABOOMCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	Super::Jump();

}

//void ABOOMCharacter::StartFire(const FInputActionValue& Value)
void ABOOMCharacter::StartFire()
{
	bIsPendingFiring = true;
	if (HasNoWeapons())
	{
		return;
	}

	if (Weapons[CurrentWeaponSlot] != nullptr)
	{
		Weapons[CurrentWeaponSlot]->HandleFireInput();
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
			HighlightedActor = nullptr;
			InteractableObject->Interact(this);
			InteractableObject->OnInteractionRangeExited(this);
		}
		this->GetNearestInteractable();
		return;
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

