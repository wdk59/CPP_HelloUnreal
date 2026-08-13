// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"
#include "ItemDataAsset.generated.h"

class APickupBase;

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class CPP_HELLOUNREAL_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public :
	
	// 아이템 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base|Data")
	FText DisplayName;

	// 아이템 설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base|Data")
	FText Description;

	// 아이템 가격
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base|Data")
	int32 Price = 1;

	// 아이템이 인벤토리 한 칸에 쌓일 수 있는 최대 개수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base|Data")
	int32 MaxStackCount = 1;

	// 아이템 스폰할 위치 Offset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base|Spawn")
	FVector SpawnLocationOffset = FVector::ZeroVector;

	// 스폰할 아이템 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base|Spawn")
	TSoftClassPtr<APickupBase> PickupClass;

public:
	virtual TSharedPtr<FStreamableHandle> RequestDataLoad(FStreamableDelegate InDelegate) const;
	virtual bool IsLoaded() const;

protected :
	virtual void OnAsyncRequest(TArray<FSoftObjectPath>& InOutArray) const;

};
