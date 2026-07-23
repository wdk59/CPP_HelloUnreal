// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test02/Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API AHealthPickup : public APickup
{
	GENERATED_BODY()

public:

	AHealthPickup();

protected:

	virtual void ChangeStat_Implementation(AActor* OtherActor) override;

};
