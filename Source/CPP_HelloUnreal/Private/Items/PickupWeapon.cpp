// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickupWeapon.h"
#include "Weapons/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"
#include "Components/SphereComponent.h"

// UPROPERTY 멤버가 바뀌거나 새로 생성되거나 할 때 실행: 블루프린트에서 메시 바뀌는 거 확인 가능
// PostInitializeComponents 이벤트도 나쁘진 않음. 하지만 블프에서 보려면 OnConstruction이 굿. 자세한 건 AActor 주석 참고.
void APickupWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SphereCollision->InitSphereRadius(200.f);
	
	if (WeaponData)
	{
		if (UStaticMesh* StaticMeshData = WeaponData->Mesh.LoadSynchronous())
		{
			Mesh->SetStaticMesh(StaticMeshData);
			Mesh->SetRelativeLocation(MeshBaseLocation + WeaponData->LocationOffset);
		}

	}

}

void APickupWeapon::OnPickup(AActor* InTarget)
{

	if (GetWorldTimerManager().IsTimerActive(PickupEffectTimerHandle))	return;	// 타이머가 이미 작동 중이면 종료 (중복 실행 방지용)

	Super::OnPickup(InTarget);

	TargetActor = InTarget;

	// 연출

	if (IsPickupEffectAssetReady())
	{
		// 더 이상의 오버랩이 발생하지 않게 하기
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		PickupElapsedTime = 0.f;
		PickupStartLocation = Mesh->GetComponentLocation();

		GetWorldTimerManager().SetTimer(
			PickupEffectTimerHandle,
			this,
			&APickupWeapon::OnUpdatePickupEffect,
			TimerInterval,
			true
		);
	}
	else
	{
		OnFinishPickupEffect();
	}

}

void APickupWeapon::OnUpdatePickupEffect()
{
	if (!TargetActor.IsValid())
	{
		OnFinishPickupEffect();
		return;
	}

	PickupElapsedTime += TimerInterval;
	float Progress = PickupElapsedTime / PickupEffectDuration;

	// 플레이어한테 가까이 오기 보간
	float DistanceAlpha = PickupAlphaCurve->GetFloatValue(Progress);
	FVector Goal = TargetActor.Get()->GetActorLocation();
	FVector NewLocation = FMath::Lerp(PickupStartLocation, Goal, DistanceAlpha);

	// 높이 조정
	float HeightOffset = PickupHeightCurve->GetFloatValue(Progress) * PickupEffectHeight;
	NewLocation.Z += HeightOffset;

	Mesh->SetWorldLocation(NewLocation);

	float Scale = PickupScaleCurve->GetFloatValue(Progress);
	Mesh->SetRelativeScale3D(FVector(Scale));

	if (Progress >= 1.f)
	{
		OnFinishPickupEffect();
	}
}

void APickupWeapon::OnFinishPickupEffect()
{
	// 연출이 완전히 종료된 후 실제 장착 실행

	GetWorldTimerManager().ClearTimer(PickupEffectTimerHandle);

	if (TargetActor.IsValid())
	{
		IWeaponUserInterface::Execute_EquipWeapon(TargetActor.Get(), WeaponData);
	}

	Destroy();
}

bool APickupWeapon::IsPickupEffectAssetReady() const
{
	return PickupAlphaCurve && PickupHeightCurve && PickupScaleCurve;
}
