// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon_Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "CoopGame.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "Weapon/Projectile/SProjectile.h"
#include "..\..\Public\Weapon\SWeapon_Projectile.h"


void ASWeapon_Projectile::Fire()
{
	if (Role < ROLE_Authority)
	{
		ServerFire();
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		//Deduce Ammo
		if (CurrentAmmo > 0)
		{
			CurrentAmmo--;
		}
		else {
			return;
		}

		//Play Sound
		UGameplayStatics::PlaySoundAtLocation(this, FireStartSound, GetActorLocation());

		//Run Function To Shoot
		SpawnProjectile(MyOwner);
	}

	PlayFireEffects_Projectile();

	LastFireTime = GetWorld()->TimeSeconds;
}


void ASWeapon_Projectile::SpawnProjectile(AActor* MyOwner)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	FRotator SpawnRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

	ProjectileSpawned = GetWorld()->SpawnActor<ASProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);

	//Set the variables for the projectile
	//ProjectileSpawned->BaseDamage = BaseDamage;
	//ProjectileSpawned->Origin = MeshComp->GetSocketLocation(MuzzleSocketName);
	//ProjectileSpawned->DamageTypeClass = DamageType;
	ProjectileSpawned->IgnoreActors.Add(this);
	ProjectileSpawned->IgnoreActors.Add(MyOwner);
	//ProjectileSpawned->DoFullDamage = DoFullDamage;
}


void ASWeapon_Projectile::PlayFireEffects_Projectile()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
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
