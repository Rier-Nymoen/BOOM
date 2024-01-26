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
#include "AI/BOOMAIController.h"
#include "Camera/CameraComponent.h"
#include "BOOM/BOOMProjectile.h"
#include "Curves/CurveFloat.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"


#include "AbilitySystemComponent.h"
#include"AbilitySystemInterface.h"

// Sets default values
ABOOMWeapon::ABOOMWeapon()
{
	//dont forget to turn off
	PrimaryActorTick.bCanEverTick = false;

	Weapon1P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh1P");
	Weapon1P->bOnlyOwnerSee = true;
	Weapon3P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh3P");
	Weapon3P->bOwnerNoSee = true;


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

	CurrentState = InactiveState;
	bVisualizeHitscanTraces = true;

	TimeCooling = 0;
	bIsOverheated = false;
	BulletsPerShot = 1;

	bHeatAffectsSpread = false;
	CurrentHeat = 0;

	SpreadGroupingExponent = 1;

	WeaponCoolingStartSeconds = 2.F;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");

	FiringSource = EFiringSource::Camera;
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

}

void ABOOMWeapon::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	AddAmmo(-AmmoCost);
	
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
	
	if (Character && Character->GetPlayerHUD()) //it was possible for the character to be set to null when collision settings allowed for self inflcited damage and dies.
	{
		Character->GetPlayerHUD()->GetWeaponInformationElement()->SetCurrentAmmoText(CurrentAmmo);
	}

	
	//Should clamp the values around the ranges of the curves incase we get an X value that is out of range of the curve it will be used as an input to.
	if(bHeatAffectsSpread)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_WeaponCooldown);
		
		CurrentHeat += HeatToHeatIncreaseCurve.GetRichCurve()->Eval(CurrentHeat);
		
		CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurve()->Eval(CurrentHeat);
		
		CurrentSpreadAngle = FMath::Clamp(CurrentSpreadAngle, MinSpreadAngle, MaxSpreadAngle);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_WeaponCooldown, this, &ABOOMWeapon::Cooldown, WeaponCoolingStartSeconds, true);
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

/*
Fire Hitscan/Projectile: Can choose between firing from actual muzzle of weapon, or camera. On AI, the firing would look weird coming from some camera. 
*/
//@todo add code to reasonably prevent some self inflicted damage
void ABOOMWeapon::FireHitscan()
{
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());

	FVector StartTrace;
	FVector EndTrace;						//should be same as boverridingcamerafiring
	if (PlayerController && FiringSource == EFiringSource::Camera)//change back to player controller
	{
		//Eventually must change these to adjust for aim based on weapon spread. 
		FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

		 StartTrace = CameraLocation;
		 
		 FVector ShotDirection = CalculateBulletSpreadDir(CameraRotation);

		 EndTrace = StartTrace + (ShotDirection * HitscanRange);
	}
	else if(GetOwner() && FiringSource == EFiringSource::OwnerCenter) 
	{
		//jank
		StartTrace = GetOwner()->GetActorLocation();
		FRotator StartRotation = GetOwner()->GetActorRotation();
		FVector ShotDirection = CalculateBulletSpreadDir(StartRotation);

		EndTrace = StartTrace + (ShotDirection * HitscanRange);
	}
	else
	{
		if (Weapon1P)
		{
			StartTrace = Weapon1P->GetSocketLocation("Muzzle");
			FRotator EndRotation = Weapon1P->GetSocketRotation("Muzzle");

			EndTrace = StartTrace * HitscanRange;
		}
	}
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(Character); //maybe instigator could be a better choice?
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, TraceParams);
	if (bVisualizeHitscanTraces)
	{
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(FMath::FRandRange(0.0F, 255.0F), FMath::FRandRange(0.0F, 255.0F), FMath::FRandRange(0.0F, 255.0F)), true, 0.4);
	}
	
	if (bHit)
	{
		/*
		Should use targeting maybe?
		*/

		if (ImpactDecal)
		{
			UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ImpactDecal, FVector(5, 5, 5), HitResult.Location);
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

		if (AbilitySystemComponent)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddHitResult(HitResult);

			FPredictionKey PredictionKey;
			if (DamageEffect)
			{
				const FGameplayEffectSpecHandle DamageEffectSpec = TargetAbilitySystemComponent->MakeOutgoingSpec(DamageEffect, 0.F, EffectContext);
				
				TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*DamageEffectSpec.Data, PredictionKey);
				UE_LOG(LogTemp, Warning, TEXT("DamageEffect Applied to HitObject"))
			}
		}
	}
}
/*@TODO: Make function to reuse firing source logic from FireHitscan in FireProjectile() at a later point*/
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
			SetInstigator(Character); //@TODO - instigators arent managed properly in this class
			SpawnedProjectile->GetCollisionComp()->MoveIgnoreActors.Add(Character);
			SpawnedProjectile->GetCollisionComp()->MoveIgnoreActors.Add(GetInstigator());
			Character->GetCapsuleComponent()->MoveIgnoreActors.Add(SpawnedProjectile);
			/*
			Possibly empty MoveIgnoreActors after x amount of time.
			*/
			UGameplayStatics::FinishSpawningActor(SpawnedProjectile, ProjectileTransform);
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

void ABOOMWeapon::Interact(ABOOMCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	SetOwner(Character);
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

void ABOOMWeapon::HandleFireInput()
{
	if (Character)
	{
		Character->bIsPendingFiring = true;
	}
	CurrentState->HandleFireInput();
}

void ABOOMWeapon::HandleStopFireInput()
{
	if (Character)
	{
		Character->bIsPendingFiring = false;
	}
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
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.F, FColor(255, 82, 255), "Start of ID: "+ GetName() + "Model Name:" + Name.ToString());

	CurrentState = NewState;

	if (ZoomMode == EZoom::Zoomed)
	{
		if (CurrentState == ReloadingState || CurrentState == EquippingState || CurrentState == UnequippingState)
		{
			ZoomOut();
		}
	}
	
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

	GotoState(UnequippingState);

	Weapon1P->SetSimulatePhysics(true);
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, false);
	BOOMPickUp->AttachToComponent(Weapon1P, AttachmentRules);


	if (Character)
	{
		Weapon1P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		/*
		On character death, velocity will be 0, need to have character have velocity on death.
		or may need Velocity and forward vector recorded before death when dying -- Solved by physics on death
		*/

		//@TODO: Need to have this affected by directional movement and rotation, for now this is okay.
		Weapon1P->SetPhysicsLinearVelocity(Character->GetActorForwardVector() * Character->GetVelocity().Size(), true);
	}

	BOOMPickUp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Character = nullptr;
	SetOwner(nullptr);
		
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

void ABOOMWeapon::Heat()
{

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
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (PlayerController)
	{
		ZoomMode = EZoom::Not_Zoomed;
		PlayerController->PlayerCameraManager->SetFOV(90);
	}
}

void ABOOMWeapon::ZoomIn()
{
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (PlayerController)
	{
		ZoomMode = EZoom::Zoomed;
		PlayerController->PlayerCameraManager->SetFOV(45);
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
			//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0F, FColor::Blue, "OnEquip");
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABOOMWeapon::HandleFireInput);
			EnhancedInputComponent->BindAction(StopFireAction, ETriggerEvent::Completed, this, &ABOOMWeapon::HandleStopFireInput);
		}

	}
}


void ABOOMWeapon::OnUnequip()
{

	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
		{

			EnhancedInputComponent->ClearActionBindings();
			//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0F, FColor::Orange, "OnUnequip");
			DisableInput(Cast<APlayerController>(PlayerController));
		}
	}
}


FVector ABOOMWeapon::CalculateBulletSpreadDir(FRotator StartRot) //This is circular spread. The idea  to use Quaternions was from Epic's Lyra project.
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

