// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test02/Pickup.h"
#include "StaminaPickup.generated.h"

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API AStaminaPickup : public APickup
{
	GENERATED_BODY()

public :

	AStaminaPickup();

protected :

	virtual void ChangeStat_Implementation(AActor* OtherActor) override;

};
