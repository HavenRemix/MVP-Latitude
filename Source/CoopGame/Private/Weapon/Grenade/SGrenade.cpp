// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenade.h"
#include "..\..\..\Public\Weapon\Grenade\SGrenade.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Player/SCharacter.h"


ASGrenade::ASGrenade()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Collision Component

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComp;

	//Projectile Movement

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 6600.0f;
	ProjectileMovement->MaxSpeed = 600.0f;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	//Projectile Mesh

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);

	InitialLifeSpan = 10.0;

	DamageCauser = this;
}


void ASGrenade::BeginPlay()
{
	//Call the base class
	Super::BeginPlay();

	OnDetonate();
}


void ASGrenade::BeginTimer()
{
	if (!DetonateTimer.IsValid())
	{
		GetWorldTimerManager().SetTimer(DetonateTimer, this, &ASGrenade::OnDetonate, ExplosionTime, false, 0.0f);
	}
}


void ASGrenade::OnDetonate()
{
	UE_LOG(LogTemp, Warning, TEXT("On Detonate has been run!"))

	if (DetonateTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(DetonateTimer);
	}

	if (ProjectileExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileExplosionEffect, GetActorTransform(), true);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}

	DamageCauser = this;

	FVector Origin = GetActorLocation();

	IgnoreActors.Add(this);
	//IgnoreActors.Add(GetOwner());

	ApplyDamage(BaseDamage, Origin, DamageRadius, DamageTypeClass, IgnoreActors, DamageCauser, InstigaterController, DoFullDamage);
}


void ASGrenade::ApplyDamage(float BaseDamage, FVector Origin, float DamageRadius, TSubclassOf<UDamageType> DamageTypeClass, TArray<AActor*> IgnoreActors, AActor* DamageCauser, APlayerController* InstigaterController, bool DoFullDamage)
{
	if (UGameplayStatics::ApplyRadialDamage(this, BaseDamage, Origin, DamageRadius, DamageTypeClass, IgnoreActors, DamageCauser, InstigaterController, DoFullDamage))
	{

	}

	Destroy();
}
