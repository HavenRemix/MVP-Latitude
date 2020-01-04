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
class ASPlayerController;
class ASGrenade;

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

	void NextWeaponInput();

	void PreviousWeaponInput();

	void YeetGrenade();

// ------- COMPONENTS ------- \\

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* FPSMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

// ------- VARIABLES ------- \\

//Player

	float DefaultFOV;

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

//Weapon

	UPROPERTY(BlueprintReadOnly, Replicated)
	ASWeapon* CurrentWeapon;

	UPROPERTY(Replicated)
	ASWeapon* UnequippedWeapon;

	UPROPERTY(Replicated)
	ASWeapon* TransactionItem;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<TSubclassOf<ASWeapon>> Weapons;

	UPROPERTY(BlueprintReadOnly, Replicated)
	ASGrenade* CurrentGrenade;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<TSubclassOf<ASGrenade>> Grenades;

	int32 GrenadeSelected;

	int WeaponNum;

	//Variable to see if the weapon type is a rifle or someting else
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool bHasRifleTypeWeapon;

	//Sockets

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponBackAttachSocketName;

	//Timers

	FTimerHandle TimerHandle_ReloadTimer;
	FTimerHandle TimerHandle_SwitchTimer;

	//Ammo

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponConfig")
	int32 MaxAmmo_Heavy;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponConfig")
	int32 MaxAmmo_Medium;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponConfig")
	int32 MaxAmmo_Light;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponConfig")
	int32 MaxAmmo_Shells;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponConfig")
	int32 MaxAmmo_Rockets;

// ------- FUNCTIONS ------- \\

	//TSubclassOf<ASWeapon> Weapon

	void EquipWeapon();
	void CreateWeapons();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCreateWeapons();

//Health

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// ------- VARIABLES ------- \\

//Weapon

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bWantsToZoom;

	UPROPERTY(BlueprintReadOnly, Category = "Online")
	bool bIsServer;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bReloading;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bSwitching;

// ------- FUNCTIONS ------- \\

	virtual FVector GetPawnViewLocation() const override;

//Weaposn

	UFUNCTION(BlueprintCallable)
	void StartFire();

	UFUNCTION(BlueprintCallable)
	void StopFire();

	UFUNCTION(BlueprintImplementableEvent)
	void BPReload();

	void Reload();

	void EndReload();

// ------- AUDIO ------- \\

	UFUNCTION(Exec)
	void PlaySong();

	UPROPERTY(EditDefaultsOnly)
	USoundCue* ReMixTrax;

private:

	class ASPlayerState* CurrentPlayerState;

// ------- FUNCTION ------- \\

	void FinishAction();

	void SetMaxAmmo_Player(FString TypeOfAmmo, int32 AmmoToSetTo);

};