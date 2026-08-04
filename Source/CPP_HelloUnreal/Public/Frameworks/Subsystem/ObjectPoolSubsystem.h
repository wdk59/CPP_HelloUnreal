// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectPoolSubsystem.generated.h"

USTRUCT()
struct FActorPool
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> Actors;
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

	AActor* Spawn(const TSubclassOf<AActor> SpawnActorType, const FTransform& InTransform);

	void ReturnPool(AActor* InActor);

protected :

	// 사용 대기 중인 액터들
	UPROPERTY(Transient)
	TMap<TSubclassOf<AActor>, FActorPool> ReadyActors;

	// 실제 사용 중인 액터들: 중간에 있을 때 비활성화될 수 있기 때문에 Array가 아닌 Set으로 저장
	UPROPERTY(Transient)
	TSet<TObjectPtr<AActor>> ActiveActors;

	UPROPERTY()
	TSubclassOf<AActor> DamagePopupType = nullptr;

	UPROPERTY()
	TSubclassOf<AActor> BloodPopupType = nullptr;
	
};
