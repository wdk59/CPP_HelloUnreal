// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test01/Teacher_FloatingActor.h"
#include "Teacher_FloatingActorByCurve.generated.h"

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API ATeacher_FloatingActorByCurve : public ATeacher_FloatingActor
{
	GENERATED_BODY()

protected :
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveFloat> HeightCurve = nullptr;

protected :
	void UpdateFloatingMesh() override;
	
};
