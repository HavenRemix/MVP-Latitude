// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/SWeapon.h"
#include "SWeapon_Instant.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASWeapon_Instant : public ASWeapon
{
	GENERATED_BODY()

protected:

	virtual void Fire() override;
	
};
