// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon_Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "CoopGame.h"


void ASWeapon_Projectile::Fire()
{
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
		SpawnProjectile();
	}
}
