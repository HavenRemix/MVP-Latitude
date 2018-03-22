// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class ASWeapon;
class USHealthComponent;
class USkeletalMeshComponent;
class AActor;
class USoundCue;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASCharacter();

protected:

	virtual void BeginPlay() override;


// ------- INPUT ------- \\


	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	void Zoom();

	void BeginRun();

	void EndRun();


// ------- COMPONENTS ------- \\


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* FPSMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USHealthComponent* HealthComp;


// ------- VARIABLES ------- \\


	float DefaultFOV;

	UPROPERTY(BlueprintReadWrite, Category = "Weapons")
		bool bWantsToZoom;

	UPROPERTY(BlueprintReadWrite, Category = "Weapons")
		bool bIsRunning;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		bool bIsAI;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
		float ZoomInterpSpeed;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
		bool bDied;

// ------- EXTERNALS ------- \\


//Weapon

	UPROPERTY(Replicated)
		ASWeapon* CurrentWeapon;

	uint16 WeaponNum;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
		FName WeaponAttachSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<ASWeapon> RifleClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<ASWeapon> TrinityClass;

	UFUNCTION(BlueprintPure)
		bool IsTargeting(bool WasTargeting);

	void EquipWeapon(uint16 WeaponNumber);

	void NextWeaponInput();
	void PreviousWeaponInput();

//Health

	UFUNCTION()
		void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


public:

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

// ------- AUDIO ------- \\

	UFUNCTION(Exec)
	void PlaySong();

	UPROPERTY(EditDefaultsOnly)
	USoundCue* ReMixTrax;

};
