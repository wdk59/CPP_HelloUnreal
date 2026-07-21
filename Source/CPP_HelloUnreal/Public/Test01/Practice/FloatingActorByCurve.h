// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test01/Practice/FloatingActor.h"
#include "Curves/CurveFloat.h"
#include "FloatingActorByCurve.generated.h"

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API AFloatingActorByCurve : public AFloatingActor
{
	GENERATED_BODY()
	
protected :
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

protected :
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveFloat> Curve = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurveDuration = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentCurveValue = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float PreCurveValue = 0.f;

protected :

	UFUNCTION(BlueprintCallable)
	void FloatingByCurve();

};
