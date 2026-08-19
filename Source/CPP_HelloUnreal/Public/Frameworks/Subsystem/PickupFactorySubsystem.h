// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/StreamableManager.h"
#include "PickupFactorySubsystem.generated.h"

class APickupBase;
class UItemDataAsset;

// 일반 Delegate가 더 빠르기 때문에 상황 맞춰서 둘 다 사용
// cf) Dynamic Delegate는 이름으로 찾아서 작동(?)
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPickupSpawnedDynamic, APickupBase*, SpawnedPickup);
DECLARE_DELEGATE_OneParam(FOnPickupSpawned, APickupBase*);
/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UPickupFactorySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public :

	// USubSystem 함수 오버라이드 -----------------------------------------------------------------
	// 서브시스템을 만들지에 대한 여부를 결정하는 함수
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// 서브시스템이 생성되었을 때 실행될 함수
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 서브시스템이 삭제될 때 실행될 함수
	virtual void Deinitialize() override;
	// --------------------------------------------------------------------------------------------

private :

	// 비동기 작업의 핸들 모음 (진행 중인 것들)
	TArray<TSharedPtr<FStreamableHandle>> ActiveStreamableHandles;

public :

	// 픽업 액터 스폰 (동기방식)
	UFUNCTION(BlueprintCallable, Category = "Factory|Pickup")
	APickupBase* SpawnPickup(const UItemDataAsset* InItemDataAsset, const FTransform& InTransform);

	// 픽업 액터 스폰 (비동기 방식, 로딩 완료 후 OnSpawned 델리게이트 호출)
	void SpawnPickupAsync(const UItemDataAsset* InItemDataAsset, const FTransform& InTransform, FOnPickupSpawned OnSpawned);
	
	UFUNCTION(BlueprintCallable, Category = "Factory|Pickup", meta = (DisplayName = "Spawn Pickup Async"))
	void K2_SpawnPickupAsync(const UItemDataAsset* InItemDataAsset, const FTransform& InTransform, FOnPickupSpawnedDynamic OnSpawned);

private :
	// 실제 스폰을 처리할 함수
	APickupBase* SpawnProcess(const UItemDataAsset* InItemDataAsset, const FTransform& InTransform);
	
	// 비동기 작업의 핸들 중 완료된 핸들을 정리하는 함수
	void CleanupCompletedHandles();

};
