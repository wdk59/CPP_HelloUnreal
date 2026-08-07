// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Datas/Items/ItemDataAsset.h"
#include "NiagaraSystem.h"	// TSoftObjectPtr로 선언하기 때문에 헤더 필요
#include "WeaponDataAsset.generated.h"

class USkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UWeaponDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public :
	
	// 무기의 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Appearance")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Appearance")
	TSoftObjectPtr<UNiagaraSystem> TrailVFX;

	// 무기가 Attache될 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|HitArea")
	FName AttachSocketName = TEXT("Weapon_R");

	// 무기 Attach할 위치 Offset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Appearance")
	FVector AttachLocationOffset = FVector::ZeroVector;

	// HitArea 캡슐의 높이 절반
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|HitArea")
	float HitAreaHalfHeight = 60.f;

	// HitArea 캡슐의 반지름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|HitArea")
	float HitAreaRadius = 15.f;

	// 무기 사용 횟수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	int32 UseCount = 10;

	// 무한 사용 가능 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	bool bInfinityUse = false;

	// 무기의 공격력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	float AttackPower = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	float AreaAttackPower = 10.f;

	// 무기의 범위 공격의 반지름 (안쪽, 이 안쪽은 데미지 100%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	float AreaAttackInnerRadius = 100.f;

	// 무기의 범위 공격의 반지름 (바깥쪽, Inner ~ Outter 범위는 거리에 다라 감소)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	float AreaAttackOuterRadius = 300.f;

	// 무기의 공격 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Data")
	TObjectPtr<UAnimMontage> AttackMontage;

protected :
	virtual void OnAsyncRequest(TArray<FSoftObjectPath>& InOutArray) const override;

public:
	virtual bool IsLoaded() const override;

};
