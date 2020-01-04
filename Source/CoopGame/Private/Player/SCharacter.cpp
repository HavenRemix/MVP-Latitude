// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/SCharacter.h"
#include "Player/SPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame.h"
#include "SHealthComponent.h"
#include "Weapon/SWeapon.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/Vector.h"
#include "GameFramework/Pawn.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"
#include "Math/UnrealMathUtility.h"
#include "Player/SPlayerState.h"
#include "..\..\Public\Player\SCharacter.h"
#include "Weapon/Grenade/SGrenade.h"



ASCharacter::ASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

//Camera Comp

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->bUsePawnControlRotation = true;

//FPS Mesh

	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FPSMesh->SetOnlyOwnerSee(true);
	FPSMesh->SetupAttachment(CameraComp);
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

//Default Mesh

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastDynamicShadow = false;
	GetMesh()->CastShadow = false;

//Defaults

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	bIsAI = false;

	WeaponAttachSocketName = "WeaponSocket";
	WeaponBackAttachSocketName = "WeaponBackSocket";

	GrenadeSelected = 0;
}


void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	PlaySong();

	if (Role >= ROLE_Authority)
	{
		CreateWeapons();
	}
}


void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);
}


void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::Zoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASCharacter::BeginRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ASCharacter::EndRun);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &ASCharacter::NextWeaponInput);
	PlayerInputComponent->BindAction("PreviousWeapon", IE_Released, this, &ASCharacter::PreviousWeaponInput);

	PlayerInputComponent->BindAction("Reload", IE_Released, this, &ASCharacter::Reload);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &ASCharacter::YeetGrenade);
}


// ------- INPUT ------- \\


void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}


void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}


void ASCharacter::BeginCrouch()
{
	Crouch();
}


void ASCharacter::EndCrouch()
{
	UnCrouch();
}


void ASCharacter::Zoom()
{
	CurrentWeapon->IsTargeting(bWantsToZoom);
}


void ASCharacter::BeginRun()
{
	if (bWantsToZoom)
	{
		CurrentWeapon->IsTargeting(bWantsToZoom);
		GetCharacterMovement()->MaxWalkSpeed = 1000;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = 1000;
	}

	bIsRunning = true;
}


void ASCharacter::EndRun()
{
	GetCharacterMovement()->MaxWalkSpeed = 600;

	bIsRunning = false;
}


void ASCharacter::StartFire()
{
	if (bIsRunning)
	{
		EndRun();
	}
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}


void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}


void ASCharacter::Reload()
{
	if (bWantsToZoom)
	{
		CurrentWeapon->IsTargeting(bWantsToZoom);
	}
	if (bIsRunning)
	{
		EndRun();
	}
	if (CurrentWeapon)
	{
		BPReload();
	}
}


void ASCharacter::EndReload()
{
	bReloading = false;
}


void ASCharacter::NextWeaponInput()
{
	if (WeaponNum == 0)
	{
		WeaponNum = 1;
	}
	else {
		WeaponNum = 0;
	}

	//int Num = WeaponNum + 1;
	//Weapons[Num]
	EquipWeapon();
}


void ASCharacter::PreviousWeaponInput()
{
	if (WeaponNum == 1)
	{
		WeaponNum = 0;
	}
	else {
		WeaponNum = 1;
	}
//	int Num = WeaponNum - 1;
	//Num = FMath::Clamp(Num - 1, 0, 2);
	//Weapons[Num]
	EquipWeapon();
}


void ASCharacter::YeetGrenade()
{
	//Spawn the grenade
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentGrenade = GetWorld()->SpawnActor<ASGrenade>(Grenades[GrenadeSelected], FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	CurrentGrenade->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);


	//Set the variables for the grenade

	CurrentGrenade->SetOwner(this);
	CurrentGrenade->IgnoreActors.Add(this);

	CurrentGrenade->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}


void ASCharacter::FinishAction()
{
	if (TimerHandle_ReloadTimer.IsValid() && CurrentWeapon)
	{
		CurrentWeapon->ReloadWeapon();
		SetMaxAmmo_Player(CurrentWeapon->AmmoTypeUsed, CurrentWeapon->MaxAmmo);

		EndReload();
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTimer);
	}

	if (TimerHandle_SwitchTimer.IsValid())
	{
		bSwitching = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_SwitchTimer);
	}
}


// ------- WEAPON LOGIC ------- \\


//TSubclassOf<ASWeapon> Weapon

//void ASCharacter::EquipWeapon()
//{
//	if (Role == ROLE_Authority)
//	{
//		if (CurrentWeapon != nullptr)
//		{
//			CurrentWeapon->Destroy();
//		}
//
//		//Spawn Rules
//		FActorSpawnParameters SpawnParams;
//		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
//		//Set Current Weapon
//		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(Weapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
//
//		//Attach the weapon to the socket
//		if (CurrentWeapon)
//		{
//			if (bIsAI)
//			{
//				CurrentWeapon->SetOwner(this);
//				CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
//			}
//			else {
//				CurrentWeapon->SetOwner(this);
//				CurrentWeapon->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
//			}
//		}
//	}
//}


void ASCharacter::EquipWeapon()
{
	//Only run if not server
	if (Role < ROLE_Authority)
	{
		ServerEquipWeapon();
	}

	//Set the variables to play the animations
	if (CurrentWeapon && UnequippedWeapon)
	{
		bSwitching = true;
		GetWorldTimerManager().SetTimer(TimerHandle_SwitchTimer, this, &ASCharacter::FinishAction, 0.1f, false, 0.3f);
	}

	//Only run if the player is the server
	if (Role >= ROLE_Authority)
	{
		//Only run if EquippedItem is valid
		if (CurrentWeapon)
		{
			//Attach the CurrentWeapon to the back of the third person mesh
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponBackAttachSocketName);
			TransactionItem = CurrentWeapon;
		}

		//Only run if UnequippedItem is valid
		if (UnequippedWeapon)
		{
			UnequippedWeapon->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}

		//Only run if TransactionItem is valid
		if (TransactionItem)
		{
			CurrentWeapon = UnequippedWeapon;
			UnequippedWeapon = TransactionItem;
		}

		CurrentWeapon->SetMaxAmmo_Weapon(MaxAmmo_Heavy, MaxAmmo_Medium, MaxAmmo_Light, MaxAmmo_Shells, MaxAmmo_Rockets);

		bHasRifleTypeWeapon = CurrentWeapon->bIsRifle;
	}
}


void ASCharacter::CreateWeapons()
{
	if (Role < ROLE_Authority)
	{
		ServerCreateWeapons();
	}

	if (Role >= ROLE_Authority)
	{
		// Spawn a default weapon query parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(Weapons[0], FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		UnequippedWeapon = GetWorld()->SpawnActor<ASWeapon>(Weapons[1], FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		//Attach to the first person mesh

		if (CurrentWeapon)
		{
			if (bIsAI)
			{
				CurrentWeapon->SetOwner(this);
				CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			}
			else {
				CurrentWeapon->SetOwner(this);
				CurrentWeapon->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			}
		}

		//Attch unequipped item to the back of the player

		if (UnequippedWeapon)
		{
			UnequippedWeapon->SetOwner(this);
			UnequippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponBackAttachSocketName);
		}

		//Set the MaxAmmo variables in the weapon to make sure we get the right information from the beggining
		if (CurrentWeapon)
		{
			CurrentWeapon->SetMaxAmmo_Weapon(MaxAmmo_Heavy, MaxAmmo_Medium, MaxAmmo_Light, MaxAmmo_Shells, MaxAmmo_Rockets);
		}
		if (UnequippedWeapon)
		{
			UnequippedWeapon->SetMaxAmmo_Weapon(MaxAmmo_Heavy, MaxAmmo_Medium, MaxAmmo_Light, MaxAmmo_Shells, MaxAmmo_Rockets);
		}
	}
}


void ASCharacter::SetMaxAmmo_Player(FString TypeOfAmmo, int32 AmmoToSetTo)
{
	//Variable so I can just use this
	int32 MaxAmmoFromWeapon = CurrentWeapon->MaxAmmo;

	if (TypeOfAmmo == "Heavy")
	{
		MaxAmmo_Heavy = MaxAmmoFromWeapon;
	}
	else if (TypeOfAmmo == "Medium")
	{
		MaxAmmo_Medium = MaxAmmoFromWeapon;
	}
	else if (TypeOfAmmo == "Light")
	{
		MaxAmmo_Light = MaxAmmoFromWeapon;
	}
	else if (TypeOfAmmo == "Shells")
	{
		MaxAmmo_Shells = MaxAmmoFromWeapon;
	}
	else if (TypeOfAmmo == "Rockets")
	{
		MaxAmmo_Rockets = MaxAmmoFromWeapon;
	}
}


//CREATE WEAPONS


bool ASCharacter::ServerCreateWeapons_Validate()
{
	//Validate as positive. Add some anti-cheat if neaded.
	return true;
}


void ASCharacter::ServerCreateWeapons_Implementation()
{
	//What it runs when the implimentation is ran after the validation is positive
	CreateWeapons();
}


//EQUIP WEAPON


bool ASCharacter::ServerEquipWeapon_Validate()
{
	//Validate as positive. Add some anti-cheat if neaded.
	return true;
}


void ASCharacter::ServerEquipWeapon_Implementation()
{
	//What it runs when the implimentation is ran after the validation is positive
	EquipWeapon();
}


// ------- FUNCTIONS ------- \\


void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		// Die!
		bDied = true;

		if (CurrentWeapon != nullptr)
		{
			CurrentWeapon->Destroy();
		}

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		SetLifeSpan(10.0f);
	}
}


FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}


void ASCharacter::PlaySong()
{
	if (IsLocallyControlled())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReMixTrax, GetActorLocation());
	}
}


void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, UnequippedWeapon);
	DOREPLIFETIME(ASCharacter, TransactionItem);
	DOREPLIFETIME(ASCharacter, bDied);
}

