// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickupBase.h"
#include "Datas/WeaponDataAsset.h"
#include "PickupWeapon.generated.h"

class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API APickupWeapon : public APickupBase
{
	GENERATED_BODY()

protected :

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void OnPickup(AActor* InTarget) override;

protected :

	// 이 픽업을 먹었을 때 획득하는 무기 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data")
	TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;

	// 아이템을 줍는 연출의 진행 상황용 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
	TObjectPtr<UCurveFloat> PickupAlphaCurve = nullptr;

	// 아이템을 줍는 연출 중 위아래로 움직임을 위한 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
	TObjectPtr<UCurveFloat> PickupHeightCurve = nullptr;

	// 아이템을 줍는 연출 중 크기 변경을 위한 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
	TObjectPtr<UCurveFloat> PickupScaleCurve = nullptr;

	// 아이템을 줍는 연출의 전체 진행 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
	float PickupEffectDuration = 0.5f;

	// PickupHeight로 인해 올라가는 높이
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
	float PickupEffectHeight = 50.f;

private :

	// 아이템을 줍는 연출용 타이머 핸들
	FTimerHandle PickupEffectTimerHandle;

	// 아이템을 줍는 대상
	TWeakObjectPtr<AActor> TargetActor = nullptr;

	// 아이템을 줍는 연출이 진행된 시간
	float PickupElapsedTime = 0.f;

	// 아이템을 줍는 연출용 타이머의 실행 간격
	const float TimerInterval = 0.02f;

	FVector PickupStartLocation;

protected :
	
	virtual void OnUpdatePickupEffect();
	virtual void OnFinishPickupEffect();

private :
	
	virtual bool IsPickupEffectAssetReady() const;

};
