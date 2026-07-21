// Fill out your copyright notice in the Description page of Project Settings.


#include "Test01/Practice/FloatingActorByCurve.h"

void AFloatingActorByCurve::BeginPlay()
{
	Super::BeginPlay();
}

void AFloatingActorByCurve::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloatingActorByCurve::FloatingByCurve()
{
	if (ElapsedTime >= CurveDuration)
	{
		ElapsedTime -= CurveDuration;
	}

	CurrentCurveValue = Curve->GetFloatValue(ElapsedTime);
	Mesh->AddRelativeLocation(FVector(0.f, 0.f, 1.f) * (CurrentCurveValue - PreCurveValue) * MoveSpeed);

	PreCurveValue = CurrentCurveValue;
}
