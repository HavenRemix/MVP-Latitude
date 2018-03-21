// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/SCharacter.h"
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
}


void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	if (Role == ROLE_Authority)
	{
		// Spawn a default weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(RifleClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
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
	IsTargeting(bWantsToZoom);
}


void ASCharacter::BeginRun()
{
	if (bWantsToZoom)
	{
		IsTargeting(bWantsToZoom);
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


// ------- WEAPON LOGIC ------- \\


bool ASCharacter::IsTargeting(bool WasTargeting)
{
	if (WasTargeting)
	{
		//Not Target Any More

		GetCharacterMovement()->MaxWalkSpeed = 600;

		FPSMesh->SetOwnerNoSee(false);
		FPSMesh->SetOnlyOwnerSee(true);

		bWantsToZoom = false;

		return false;
	}
	else
	{
		//Start To Target

		GetCharacterMovement()->MaxWalkSpeed = 300;

		FPSMesh->SetOwnerNoSee(true);
		FPSMesh->SetOnlyOwnerSee(true);

		bWantsToZoom = true;

		return true;
	}

	return false;
}


void ASCharacter::EquipWeapon(uint16 WeaponNumber)
{
	if (Role == ROLE_Authority)
	{
		if (CurrentWeapon != nullptr)
		{
			CurrentWeapon->Destroy();
		}

		// Spawn a weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//Switch to spawn a different weapon each change
		TSubclassOf<ASWeapon> WeaponToEquip;
		switch (WeaponNumber)
		{
		case 1:
			WeaponToEquip = RifleClass;
			break;

		case 2:
			WeaponToEquip = TrinityClass;
			break;

		default:
			WeaponToEquip = TrinityClass;
			break;
		}

		//Set Current Weapon
		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(WeaponToEquip, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		//Attach the weapon to the socket
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
	}
}


void ASCharacter::NextWeaponInput()
{
	EquipWeapon(2);
}


void ASCharacter::PreviousWeaponInput()
{
	EquipWeapon(1);
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


void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bDied);
}


void ASCharacter::PlaySong(float WhichSong)
{
	if (IsLocallyControlled())
	{
		if (WhichSong == 1)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SongOne, GetActorLocation());
		}
		else if (WhichSong == 2)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SongTwo, GetActorLocation());
		}
		else {
			UGameplayStatics::PlaySoundAtLocation(this, SongOne, GetActorLocation());
		}
	}
}