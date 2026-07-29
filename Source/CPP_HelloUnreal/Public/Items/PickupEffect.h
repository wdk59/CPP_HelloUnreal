// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickupBase.h"
#include "PickupEffect.generated.h"

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API APickupEffect : public APickupBase
{
	GENERATED_BODY()
	
protected :

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Stamina = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 0.0f;
	
protected :
	virtual void OnPickup(AActor* InTarget) override;

};
