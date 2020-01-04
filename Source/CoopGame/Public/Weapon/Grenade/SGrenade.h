// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGrenade.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGrenade : public AActor
{
	GENERATED_BODY()

public:

	ASGrenade();

protected:

	virtual void BeginPlay();

// ------- COMPONENTS ------- \\

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

// ------- VARIABLES ------- \\

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* ProjectileExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* ProjectileEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	class USoundBase* ExplosionSound;

	FTimerHandle DetonateTimer;

	//Grenade Configuration

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float DamageRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	bool DoFullDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float ExplosionTime;

// ------- FUNCTIONS ------- \\

	void ApplyDamage(float BaseDamage, FVector Origin, float DamageRadius, TSubclassOf<UDamageType> DamageTypeClass, TArray<AActor*> IgnoreActors, AActor* DamageCauser, APlayerController* InstigaterController, bool DoFullDamage);

	void OnDetonate();

	void BeginTimer();

public:

	TArray<AActor*> IgnoreActors;

	AActor* DamageCauser;

	APlayerController* InstigaterController;

	float LifeSpan;
};
