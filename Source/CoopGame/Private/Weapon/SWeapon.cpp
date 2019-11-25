// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/SWeapon.h"
#include "Player/SCharacter.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "GameFramework/Character.h"


ASWeapon::ASWeapon()
{
	//Defaults

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	CurrentAmmo = 60;
	MaxAmmo = 540;
	ClipSize = 60;

	BaseDamage = 20.0f;
	DefaultBulletSpread = 2.0f;
	AimBulletSpread = 0.0f;
	BulletSpread = DefaultBulletSpread;

	RateOfFire = 600;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	bIsProjectile = false;
}


void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}


// ------- FUNCTIONS ------- \\


//FIRE


void ASWeapon::Fire()
{
	// Trace the world, from pawn eyes to crosshair location

	if (Role < ROLE_Authority)
	{
		ServerFire();
	}
}


void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}


void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}


//RELOAD


void ASWeapon::Reload()
{
	if (Role < ROLE_Authority)
	{
		ServerReload();
	}

	int Temp;
	if (MaxAmmo <= 0)
	{
		return;
	}
	if (MaxAmmo >= ClipSize)
	{
		Temp = ClipSize - CurrentAmmo;
		CurrentAmmo = ClipSize;
		MaxAmmo = MaxAmmo - Temp;
		Temp = 0;

		return;
	}
	if (MaxAmmo < ClipSize)
	{
		Temp = CurrentAmmo - MaxAmmo;
		if (Temp <= ClipSize)
		{
			CurrentAmmo = CurrentAmmo + MaxAmmo;
			MaxAmmo = 0;
			Temp = 0;
			return;
		}
		if (Temp > ClipSize)
		{
			MaxAmmo = MaxAmmo - CurrentAmmo;
			CurrentAmmo = ClipSize;
		}
	}
}


void ASWeapon::ReloadWeapon()
{
	Reload();
}


//AMMO


void ASWeapon::SetMaxAmmo_Weapon(int32 MaxAmmoFromPlayer_Heavy, int32 MaxAmmoFromPlayer_Medium, int32 MaxAmmoFromPlayer_Light, int32 MaxAmmoFromPlayer_Shells, int32 MaxAmmoFromPlayer_Rockets)
{
	if (AmmoTypeUsed == "Heavy")
	{
		MaxAmmo = MaxAmmoFromPlayer_Heavy;
	}
	else if (AmmoTypeUsed == "Medium")
	{
		MaxAmmo = MaxAmmoFromPlayer_Medium;
	}
	else if (AmmoTypeUsed == "Light")
	{
		MaxAmmo = MaxAmmoFromPlayer_Light;
	}
	else if (AmmoTypeUsed == "Shells")
	{
		MaxAmmo = MaxAmmoFromPlayer_Shells;
	}
	else if (AmmoTypeUsed == "Rockets")
	{
		MaxAmmo = MaxAmmoFromPlayer_Rockets;
	}
}


//PARTICE EFFECTS


void ASWeapon::PlayFireEffects(FVector TraceEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}


void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}


// ------- SERVER FUNCTIONS ------- \\


//FIRE


void ASWeapon::ServerFire_Implementation()
{
	Fire();
}


bool ASWeapon::ServerFire_Validate()
{
	return true;
}


//RELOAD


void ASWeapon::ServerReload_Implementation()
{
	Reload();
}


bool ASWeapon::ServerReload_Validate()
{
	return true;
}


//PARTICLE EFFECTS


void ASWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic FX
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}


//REPLICATION


void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}


// ------- EXTRA ------- \\


bool ASWeapon::IsTargeting(bool WasTargeting)
{
	ASCharacter* MyOwner = Cast<ASCharacter>(GetOwner());
	if (!ensure(MyOwner != nullptr)) return false;

	if (WasTargeting)
	{
		//Not Target Any More
		MyOwner->GetCharacterMovement()->MaxWalkSpeed = 600;
		MyOwner->bWantsToZoom = false;

		BulletSpread = DefaultBulletSpread;

		return false;
	}
	else
	{
		//Start To Target
		MyOwner->GetCharacterMovement()->MaxWalkSpeed = 300;
		MyOwner->bWantsToZoom = true;

		BulletSpread = AimBulletSpread;

		return true;
	}

	return false;
}

