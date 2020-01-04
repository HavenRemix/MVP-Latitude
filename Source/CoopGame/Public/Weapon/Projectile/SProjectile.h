// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectile.generated.h"

UCLASS()
class COOPGAME_API ASProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(float BaseDamage, FVector Origin, float DamageRadius, TSubclassOf<UDamageType> DamageTypeClass, TArray<AActor*> IgnoreActors, AActor* DamageCauser, APlayerController* InstigaterController, bool DoFullDamage);

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyDamageBP();

	void OnDetonate();

// ------- VARIABLES ------- \\

	UPROPERTY(EditDefaultsOnly)
	float LifeTime;

	UPROPERTY(EditDefaultsOnly)
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly)
	float DamageRadius;

	FVector Origin;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageType> DamageTypeClass;

	TArray<AActor*> IgnoreActors;

	AActor* DamageCauser;

	UPROPERTY(EditDefaultsOnly)
	APlayerController* InstigaterController;

	UPROPERTY(EditDefaultsOnly)
	bool DoFullDamage;
};
