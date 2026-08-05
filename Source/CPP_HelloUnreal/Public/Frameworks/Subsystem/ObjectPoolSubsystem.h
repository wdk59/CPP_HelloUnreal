// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectPoolSubsystem.generated.h"

class UObjectPoolDataAsset;

// 오브젝트 풀 하나를 나타낼 구조체     
USTRUCT()
struct FObjectPool
{
	// USTRUCT()나 UCLASS()를 붙였으면 GENERATED_BODY()가 있어야
	// .generated.h에 코드가 생성돼 언리얼에서 제공하는 기능을 사용할 수 있게 됨
	GENERATED_BODY()

	// 사용 대기 중인 액터들
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> ReadyActors;

	// 실제 사용 중인 액터들
	UPROPERTY(Transient)
	TSet<TObjectPtr<AActor>> ActiveActors;

	// 초기 생성 개수
	int32 InitialSize = 0;
	
};

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UObjectPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public :

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	bool RegisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset, bool bWarmup = false);
	
	UFUNCTION(BlueprintCallable)
	bool UnRegisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset);

	UFUNCTION(BlueprintCallable)
	void Warmup(TSubclassOf<AActor> InClass);

	UFUNCTION(BlueprintCallable)
	void WarmupAll();

	UFUNCTION(BlueprintCallable)
	void ClearPool(TSubclassOf<AActor> InClass);
	
	UFUNCTION(BlueprintCallable)
	void ClearAllPools();

	UFUNCTION(BlueprintCallable)
	AActor* Spawn(TSubclassOf<AActor> InClassType, const FTransform& InTransform);

	// AActor* Spawn()에 대한 호출용
	// C++ 클래스 전용
	template<typename T>
	T* Spawn(TSubclassOf<T> InClassType, const FTransform& InTransform)
	{
		return Cast<T>(Spawn(TSubclassOf<AActor>(InClassType), InTransform));
	}

	template<typename T>
	T* Spawn(const FTransform& InTransform)
	{
		return Cast<T>(Spawn(T::StaticClass(), InTransform));
	}

	UFUNCTION(BlueprintCallable)
	void ReturnPool(AActor* InActor);

protected :

	UPROPERTY(Transient)
	TMap<const TSubclassOf<AActor>, FObjectPool> ObjectPools;
	
};
