// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "..\..\..\Public\Weapon\Projectile\SProjectile.h"
#include "Player/SCharacter.h"

// Sets default values
ASProjectile::ASProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Collision Component

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASProjectile::OnHit);

	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComp;

	//Projectile Movement

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 600.0f;
	ProjectileMovement->MaxSpeed = 600.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	//Projectile Mesh

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);

	InitialLifeSpan = LifeTime;

	DamageCauser = this;
}

// Called when the game starts or when spawned
void ASProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(ProjectileEffect, ProjectileMesh);
	}
}

// Called every frame
void ASProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}


void ASProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	OnDetonate();
}


void ASProjectile::ApplyDamage(float BaseDamage, FVector Origin, float DamageRadius, TSubclassOf<UDamageType> DamageTypeClass, TArray<AActor*> IgnoreActors, AActor* DamageCauser, APlayerController* InstigaterController, bool DoFullDamage)
{
	if (UGameplayStatics::ApplyRadialDamage(this, BaseDamage, Origin, DamageRadius, DamageTypeClass, IgnoreActors, DamageCauser, InstigaterController, DoFullDamage))
	{

	}

	Destroy();
}


void ASProjectile::OnDetonate()
{
	if (ProjectileExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileExplosionEffect, GetActorTransform(), true);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}

	Origin = GetActorLocation();

	IgnoreActors.Add(this);
	//IgnoreActors.Add(GetOwner());

	DamageCauser = this;

	ApplyDamage(BaseDamage, Origin, DamageRadius, DamageTypeClass, IgnoreActors, DamageCauser, InstigaterController, DoFullDamage);
	//ApplyDamageBP();
}
