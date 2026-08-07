// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Datas/Items/ItemDataAsset.h"
#include "MiscItemDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class CPP_HELLOUNREAL_API UMiscItemDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:

	// 아이템 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Misc|Appearance")
	TSoftObjectPtr<UStaticMesh> Mesh;

protected:
	virtual void OnAsyncRequest(TArray<FSoftObjectPath>& InOutArray) const override;

public:
	virtual bool IsLoaded() const override;

};
