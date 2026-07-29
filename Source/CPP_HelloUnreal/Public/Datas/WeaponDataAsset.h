// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"
#include "WeaponDataAsset.generated.h"

class UStaticMesh;

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public :
	
	// 무기의 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TSoftObjectPtr<UStaticMesh> Mesh;

	// 무기가 Attache될 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitArea")
	FName AttachSocketName = TEXT("Weapon_R");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitArea")
	FVector LocationOffset = FVector::ZeroVector;

	// HitArea 캡슐의 높이 절반
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitArea")
	float HitAreaHeight = 60.f;

	// HitArea 캡슐의 반지름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitArea")
	float HitAreaRadius = 30.f;

	// 무기의 공격력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	float AttackPower = 10.f;

public :
	TSharedPtr<FStreamableHandle> RequestDataLoad(FStreamableDelegate InDelegate) const;
	bool IsLoaded() const;

};
