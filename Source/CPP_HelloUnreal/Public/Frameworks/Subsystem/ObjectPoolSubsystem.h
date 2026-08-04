// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectPoolSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UObjectPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public :

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	AActor* Spawn(const FTransform& InTransform);

	void ReturnPool(AActor* InActor);

protected :

	// 사용 대기 중인 액터들
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> ReadyActors;

	// 실제 사용 중인 액터들: 중간에 있을 때 비활성화될 수 있기 때문에 Array가 아닌 Set으로 저장
	UPROPERTY(Transient)
	TSet<TObjectPtr<AActor>> ActiveActors;

	UPROPERTY()
	TSubclassOf<AActor> DamagePopupClass = nullptr;
	
};
