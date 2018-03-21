// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraComponent;
class USoundCue;

// Contains information of a single hitscan weapon linetrace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
		FVector_NetQuantize TraceTo;
};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()

public:

	ASWeapon();

protected:

	virtual void BeginPlay() override;

	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
		FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
		FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
		UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
		UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
		UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
		UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Trinity")
		TSubclassOf<UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Trinity")
		float BaseDamage;

	void Fire();

	UFUNCTION(BlueprintImplementableEvent)
		void SpawnProjectile();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;

	/* RPM - Bullets per minute fired by weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Trinity")
		float RateOfFire;

	/* Bullet Spread in Degrees */
	UPROPERTY(EditDefaultsOnly, Category = "Trinity", meta = (ClampMin = 0.0f))
		float BulletSpread;

	// Derived from RateOfFire
	float TimeBetweenShots;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
		FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

	UPROPERTY(EditDefaultsOnly)
	bool bIsProjectile;

// ------- AUDIO ------- \

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* FireStartSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* FireEndSound;

public:

	void StartFire();

	void StopFire();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* ADSCameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* MeshComp;
};
