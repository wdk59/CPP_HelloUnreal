// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickupBase.h"
#include "Datas/WeaponDataAsset.h"
#include "PickupWeapon.generated.h"

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API APickupWeapon : public APickupBase
{
	GENERATED_BODY()

protected :

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UWeaponDataAsset> WeaponData;

protected :
	virtual void OnPickup(AActor* InTarget) override;


	
};
