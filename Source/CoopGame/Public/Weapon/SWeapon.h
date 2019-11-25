// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"


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
	class USkeletalMeshComponent* MeshComp;

// ------- VARIBLES ------- \\

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly)
	bool bIsProjectile;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float RateOfFire;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (ClampMin = 0.0f))
	float AimBulletSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (ClampMin = 0.0f))
	float DefaultBulletSpread;

	float BulletSpread;

	float LastFireTime;

	float TimeBetweenShots;

// ------- FUNCTIONS ------- \\

	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	virtual void Fire();

	void Reload();

// ------- SERVER FUNCTIONS ------- \\

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReload();

	UFUNCTION()
	void OnRep_HitScanTrace();

// ------- AUDIO ------- \\

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* FireStartSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* FireEndSound;

// ------- REFERENCES ------- \\

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UPROPERTY(EditDefaultsOnly, Category = "Trinity")
	TSubclassOf<class UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	TSubclassOf<UDamageType> DamageType;

	FTimerHandle TimerHandle_TimeBetweenShots;

// ------- EMMITERS ------- \\

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	class UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	class UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trinity")
	class UParticleSystem* TracerEffect;

public:

// ------- VARIABLES ------- \\

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FString AmmoTypeUsed;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponConfig")
	int32 CurrentAmmo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponConfig")
	int32 ClipSize;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponConfig")
	int32 MaxAmmo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponConfig")
	bool IsAuto;

	bool bIsRifle;

// ------- FUNCTIONS ------- \\

	UFUNCTION()
	virtual bool IsTargeting(bool WasTargeting);

	virtual void StartFire();

	virtual void StopFire();

	UFUNCTION(BlueprintCallable)
	virtual void ReloadWeapon();

	void SetButtetSpread(float NewSpread, bool bIsAdvanced, bool bUseDefault);

	void SetMaxAmmo_Weapon(int32 MaxAmmoFromPlayer_Heavy, int32 MaxAmmoFromPlayer_Medium, int32 MaxAmmoFromPlayer_Light, int32 MaxAmmoFromPlayer_Shells, int32 MaxAmmoFromPlayer_Rockets);
};
