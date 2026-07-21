// Fill out your copyright notice in the Description page of Project Settings.

#include "Test01/Teacher_FloatingActorByCurve.h"

void ATeacher_FloatingActorByCurve::UpdateFloatingMesh()
{
	if (HeightCurve)
	{
		float CurveInput = FMath::Fmod(GetElapsedTime() / MoveDuration, 1.0f);
		float CurveValue = HeightCurve->GetFloatValue(CurveInput);
	
		Mesh->SetRelativeLocation(FVector(0, 0, GetHeightOffset() + CurveValue * MoveHeight));
	}
	else
	{
		// 커브가 없으면 부모 움직임 따라가기
		Super::UpdateFloatingMesh();
	}
}
