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


// ------- COMPONENTS ------- \\


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;


// ------- FUNCTIONS ------- \\


	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	void Fire();

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnProjectile();


// ------- SERVER FUNCTIONS ------- \\


	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UFUNCTION()
	void OnRep_HitScanTrace();

// ------- EMMITERS ------- \\


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	UParticleSystem* TracerEffect;


// ------- VARIABLES ------- \\


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Trinity")
	float BaseDamage;

	float LastFireTime;

	UPROPERTY(EditDefaultsOnly, Category = "Trinity")
	float RateOfFire;

	UPROPERTY(EditDefaultsOnly, Category = "Trinity", meta = (ClampMin = 0.0f))
	float BulletSpread;

	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly)
	bool bIsProjectile;


// ------- REFERENCES ------- \\


	UPROPERTY(EditDefaultsOnly, Category = "Trinity")
	TSubclassOf<UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	TSubclassOf<UDamageType> DamageType;

	FTimerHandle TimerHandle_TimeBetweenShots;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;


// ------- AUDIO ------- \\


	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* FireStartSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* FireEndSound;

public:

	void StartFire();

	void StopFire();


// ------- EXTRA ------- \\


	UFUNCTION()
	bool IsTargeting(bool WasTargeting);
};
