// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BOOMWeapon.h"
#include "BOOMPickUpComponent.h"
#include "Character/BOOMCharacter.h"
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
//#include "AI/BOOMAIController.h"
#include "Camera/CameraComponent.h"
#include "BOOM/BOOMProjectile.h"
#include "Curves/CurveFloat.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"

#include "AbilitySystemComponent.h"
#include"AbilitySystemInterface.h"

// Sets default values
ABOOMWeapon::ABOOMWeapon()
{
	//dont forget to turn off
	PrimaryActorTick.bCanEverTick = true;

	Weapon3P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh3P");
	Weapon3P->bOwnerNoSee = true;
	Weapon3P->SetupAttachment(RootComponent);

	Weapon1P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh1P");
	Weapon1P->SetupAttachment(Weapon3P);
	Weapon1P->bOnlyOwnerSee = true;


	BOOMPickUp = CreateDefaultSubobject<UBOOMPickUpComponent>("PickUpComponent");
	BOOMPickUp->SetupAttachment(Weapon3P);
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

	CurrentState = InactiveState;
	bVisualizeHitscanTraces = true;
	bUseHeatBasedSpreadRecoil = false;

	BulletsPerShot = 1;

	CurrentHeat = 0.f;
	MinSpreadAngle = 0.0f;
	MaxSpreadAngle = 5.f;

	CurrentRotation = FRotator::ZeroRotator;
	
	SpreadGroupingExponent = 1;

	WeaponCoolingStartSeconds = 2.F;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");

	FiringSource = EFiringSource::Camera;
	bIsPendingFiring = false;

}

// Called when the game starts or when spawned
void ABOOMWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (Character == nullptr)
	{
		CurrentState = InactiveState;
	}
	bGenerateOverlapEventsDuringLevelStreaming = true;
}

void ABOOMWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABOOMWeapon::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	//Update the time weapon was fired.
	LastTimeFiredSeconds = GetWorld()->GetTimeSeconds();

	//need object pooling solution before implementing multiple projectiles per shot.
	if (Projectile)
	{
		FireProjectile();
	}
	else
	{
		FireHitscan();
	}	

	AddAmmo(-AmmoCost);

	if (Character && Character->GetPlayerHUD()) //it was possible for the character to be set to null when collision settings allowed for self inflcited damage and dies.
	{
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);
	}			

	CurrentHeat += HeatToHeatIncreaseCurve.GetRichCurve()->Eval(CurrentHeat);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_WeaponCooldown, this, &ABOOMWeapon::Cooldown, WeaponCoolingStartSeconds, true);

	if (SpreadMode == ESpreadMode::HeatBasedSpread)
	{
		CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurve()->Eval(CurrentHeat);
		CurrentSpreadAngle = FMath::Clamp(CurrentSpreadAngle, MinSpreadAngle, MaxSpreadAngle);
	}
	else
	{
		CurrentSpreadAngle = MaxSpreadAngle;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (PlayerController)
	{
		if (FiringCameraShakeClass)
		{
			PlayerController->PlayerCameraManager->StartCameraShake(FiringCameraShakeClass, 1.f);
		}

		if (RecoilIndex == 0)
		{
			FirstShotRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		}
		
		if (RecoilPattern.IsValidIndex(RecoilIndex))
		{
			TargetRotation += RecoilPattern[RecoilIndex];
			RecoilIndex++;
			RecoilIndex = RecoilIndex % RecoilPattern.Num();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("INVALID RECOIL INDEX: %d"), RecoilIndex);
		}

	}
}

bool ABOOMWeapon::IsIntendingToRefire()
{
	if (bIsPendingFiring && HasAmmo())
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

void ABOOMWeapon::FireHitscan()
{
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());

	FVector StartTrace;
	FVector EndTrace;						
	if (PlayerController && FiringSource == EFiringSource::Camera)
	{
		FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

		 StartTrace = CameraLocation;
		 
		 FVector ShotDirection = CalculateBulletSpreadDir(CameraRotation);

		 EndTrace = StartTrace + (ShotDirection * HitscanRange);
	}
	else if(GetOwner() && FiringSource == EFiringSource::OwnerCenter) 
	{
		StartTrace = GetOwner()->GetActorLocation();
		FRotator StartRotation = GetOwner()->GetActorRotation();

		AAIController* AIController = Cast<AAIController>(Character->GetController());
		if (AIController)
		{
			StartRotation += FRotator(0, Character->BurstGeometryProperties.CurrentBurstAngle, 0);
		}

		FVector ShotDirection = CalculateBulletSpreadDir(StartRotation);

		EndTrace = StartTrace + (ShotDirection * HitscanRange);
	}
	else
	{
		if (Weapon1P)
		{
			//@TODO Add: SocketNameMuzzle member with this value as default.
			StartTrace = Weapon1P->GetSocketLocation("Muzzle");
			FRotator EndRotation = Weapon1P->GetSocketRotation("Muzzle");
			EndTrace = StartTrace  + EndRotation.Vector() * HitscanRange;
		}
	}

	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(Character);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, TraceParams);
	if (bVisualizeHitscanTraces)
	{
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(0,120,160, CurrentAmmo/MagazineSize), false, FireRateSeconds);
	}
	
	if (bHit)
	{
		if (ImpactDecal)
		{
			UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ImpactDecal, FVector(1, 1, 1), HitResult.Location, HitResult.Normal.Rotation());
			Decal->SetFadeScreenSize(0.F);
		}

		AActor* HitActor = HitResult.GetActor();
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(HitActor);

		if (!AbilitySystemInterface)
		{
			return;
		}

		UAbilitySystemComponent* TargetAbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
		if (!TargetAbilitySystemComponent)
		{
			return;
		}
		check(GetInstigator())
		if (AbilitySystemComponent)
		{
			if (DamageEffect)
			{
				FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
				EffectContext.AddHitResult(HitResult);
				EffectContext.AddInstigator(GetInstigator(), this);

				FPredictionKey PredictionKey;
				const FGameplayEffectSpecHandle DamageEffectSpec = TargetAbilitySystemComponent->MakeOutgoingSpec(DamageEffect, 0.F, EffectContext);

				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*DamageEffectSpec.Data, TargetAbilitySystemComponent);
			}
		}
	}
}

void ABOOMWeapon::FireProjectile()
{
	UWorld* const World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		FVector ProjectileSpawnLocation;
		FRotator ProjectileSpawnRotation;
		if (PlayerController && FiringSource == EFiringSource::Camera)
		{
			FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
			FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

			ProjectileSpawnLocation = CameraLocation;
			ProjectileSpawnRotation = CameraRotation;
		}
		else 
		{
			FVector MuzzleLocation = Weapon1P->GetSocketLocation("Muzzle");
			FRotator MuzzleRotation = Weapon1P->GetSocketRotation("Muzzle");
			
			ProjectileSpawnLocation = MuzzleLocation;
			ProjectileSpawnRotation = MuzzleRotation;
		}

		FActorSpawnParameters ProjectileSpawnParams;

		ProjectileSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ProjectileSpawnRotation = CalculateBulletSpreadDir(ProjectileSpawnRotation).Rotation();
		
		FTransform ProjectileTransform(ProjectileSpawnRotation, ProjectileSpawnLocation);
		ABOOMProjectile* SpawnedProjectile = Cast<ABOOMProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, Projectile, ProjectileTransform));

		if (SpawnedProjectile)
		{
			SpawnedProjectile->SetOwner(this);
			SpawnedProjectile->GetCollisionComp()->MoveIgnoreActors.Add(Character);
			//SpawnedProjectile->GetCollisionComp()->MoveIgnoreActors.Add(GetInstigator());
			Character->GetCapsuleComponent()->MoveIgnoreActors.Add(SpawnedProjectile);
			/*@TODO: Possibly empty MoveIgnoreActors after x amount of time.*/
			UGameplayStatics::FinishSpawningActor(SpawnedProjectile, ProjectileTransform);
		}
	}
}


void ABOOMWeapon::HandleReloadInput()
{
	if (CurrentState != nullptr)
	{
		bIsPendingFiring = false;
		CurrentState->HandleReloadInput();
	}
}

void ABOOMWeapon::ReloadWeapon()
{
	if (CanReload())
	{
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

void ABOOMWeapon::FeedReloadWeapon()
{
	CurrentAmmo++;
	CurrentAmmoReserves--;
	check(Character)
	if (Character->GetPlayerHUD())
	{
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetReserveAmmoText(CurrentAmmoReserves);
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);
	}
}

//When the weapon is added to your equipment
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

	if(TargetCharacter->GetPlayerHUD())
	{
		FItemInformation* WeaponData = nullptr;
		if (TargetCharacter->WeaponTable)
		{
			WeaponData = TargetCharacter->WeaponTable->FindRow<FItemInformation>(Name, cont, false);
		}

		TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetPromptText(Name);
		if(WeaponData)
		{
			TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetPromptImage(WeaponData->ItemImage);
		}
		TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABOOMWeapon::OnInteractionRangeExited(ABOOMCharacter* TargetCharacter)
{
	if (TargetCharacter->GetPlayerHUD())
	{
		TargetCharacter->GetPlayerHUD()->GetPickUpPromptElement()->SetVisibility(ESlateVisibility::Hidden);

	}
}
/*reworking input for firing weapons*/
void ABOOMWeapon::HandleFireInput()
{
	bIsPendingFiring = true;
	CurrentState->HandleFireInput();
}

void ABOOMWeapon::HandleSingleFireInput()
{
	CurrentState->HandleFireInput();
}

void ABOOMWeapon::HandleStopFireInput()
{
	bIsPendingFiring = false;
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

	if (ZoomMode == EZoom::Zoomed)
	{
		if (CurrentState == ReloadingState || CurrentState == EquippingState || CurrentState == UnequippingState)
		{
			ZoomOut();
		}
	}
	
	CurrentState->EnterState();	
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

float ABOOMWeapon::GetLastTimeFiredSeconds()
{
	return LastTimeFiredSeconds;
}

bool ABOOMWeapon::IsReadyToFire()
{
	if ((GetWorld()->GetTimeSeconds() - LastTimeFiredSeconds) >= (FireRateSeconds))
	{
		return true;
	}
	return false;
}

void ABOOMWeapon::HandleBeingDropped()
{
	FDetachmentTransformRules DetRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetRules);


	Weapon3P->SetSimulatePhysics(true);
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, false);
	BOOMPickUp->AttachToComponent(Weapon3P, AttachmentRules);


	if (Character)
	{
		Weapon3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		//@TODO: Need to have this affected by directional movement and rotation, for now this is okay.
		Weapon3P->SetPhysicsLinearVelocity(Character->GetActorForwardVector() * Character->GetVelocity().Size(), true);
	}

	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Character = nullptr;
	SetOwner(nullptr);

	GotoState(UnequippingState);

		
}

void ABOOMWeapon::DisableCollision()
{
	Weapon3P->SetSimulatePhysics(false);
	Weapon3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BOOMPickUp->SetSimulatePhysics(false);
	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ABOOMWeapon::CanReload()
{
	return CurrentAmmoReserves > 0 && CurrentAmmo < MagazineSize;
}

void ABOOMWeapon::Zoom()
{
	if (ZoomMode == EZoom::Zoomed)
	{
		ZoomOut();
		return;
	}
	if(CurrentState != ReloadingState && CurrentState != EquippingState && CurrentState != UnequippingState)
	{
		ZoomIn();
	}
}

void ABOOMWeapon::ZoomOut()
{
	if (Character)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		if (PlayerController)
		{
			ZoomMode = EZoom::Not_Zoomed;
			PlayerController->PlayerCameraManager->SetFOV(90);
		}
	}
}

void ABOOMWeapon::ZoomIn()
{
	if (Character)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		if (PlayerController)
		{
			ZoomMode = EZoom::Zoomed;
			PlayerController->PlayerCameraManager->SetFOV(45);
		}
	}

}

void ABOOMWeapon::Cooldown()
{
	float CooldownRate = HeatToCooldownCurve.GetRichCurve()->Eval(CurrentHeat);
	CurrentHeat = FMath::Clamp(CurrentHeat - CooldownRate, 0.F , 100.F);

	if(CurrentHeat == 0 )
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_WeaponCooldown);
	}
	
}

//This is technically "wielding" the weapon.
void ABOOMWeapon::OnEquip()
{
	/*
	The advantage of having the player bind their own inputs is to avoid the complexity of binding inputs say in - weapons with varying input cases, and vehicles.
	Even if you used an interactable interface, I believe managing the complexity this way is easier.
	*/
	if (Character == nullptr)
	{
		return;
	}
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		EnableInput(PlayerController);
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABOOMWeapon::HandleFireInput);
			EnhancedInputComponent->BindAction(StopFireAction, ETriggerEvent::Completed, this, &ABOOMWeapon::HandleStopFireInput);
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ABOOMWeapon::HandleReloadInput);
			EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Started, this, &ABOOMWeapon::Zoom);

		}

	}
}

//This is technically "unwielding" the weapon.
void ABOOMWeapon::OnUnequip()
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		PlayerController->PlayerCameraManager->StopAllInstancesOfCameraShake(FiringCameraShakeClass, true);
		if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
		{

			EnhancedInputComponent->ClearActionBindings();
			DisableInput(Cast<APlayerController>(PlayerController));
		}
	}
}


FVector ABOOMWeapon::CalculateBulletSpreadDir(FRotator StartRot) //This is circular spread. The idea  to use Quaternions to define the rotations was from Epic's Lyra project.
{	
	//Imagining an axis of rotation around the weapon's barrel, this is will represent where on a spread circle the bullet will land. (Magnitude of Rotation)
	const float AngleAround = FMath::FRand() * 360;

	//Angle of deviation from the barrel (Affects Radius of spread circle)
	const float HalfAngle = FMath::Pow(FMath::FRand(), SpreadGroupingExponent) *  CurrentSpreadAngle / 2;

	//WorldRotation of Weapon muzzle.
	 FQuat StartQuat(StartRot);

	 FQuat HalfAngleQuat(FRotator(HalfAngle, 0,0)); 

	 FQuat AngleAroundQuat(FRotator(0, 0, AngleAround));

	//Has to be done right to left: Rotate to origin of shot <-- Rotate vector along axis  <-- Apply Deviation
	 FQuat FinalQuat = StartQuat * AngleAroundQuat* HalfAngleQuat;

	//I believe with how Epic implements quaternions, the Identity Quaterion is  cos(0) + sin(0)(1,0,0).
	/*
	
	 https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html According to this resource: "There is one special case in which equation (6) will fail.
	 A quaternion with the value q = (1,0,0,0) is known as the identity quaternion, and will produce no rotation.
	 In this case, equation (5) will produce a rotation angle (theta) of zero, which is what we expect. 
	 However, since the axis of rotation is undefined when there is no rotation, equation (6) will generate a divide-by-zero error.
	 Any software implementation should therefore test whether q0 equals 1.0, and if it does should set theta = 0, and (xhat, yhat, zhat) = (1, 0, 0)."
	
	*/
	/*
	So if we treat the vector (1,0,0) as the starting axis of rotation of the quaternion, then rotating the vector by this quaternion should yield an angle of axis matching the operations we took.
	*/


	FVector FinalVector = FinalQuat.RotateVector(FVector(1, 0, 0));


	return FinalVector;
}
