// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/Items/ItemDataAsset.h"
#include "Engine/AssetManager.h"
#include "Items/PickupBase.h"

TSharedPtr<FStreamableHandle> UItemDataAsset::RequestDataLoad(FStreamableDelegate InDelegate) const
{
	TArray<FSoftObjectPath> TargetsToLoad;
	OnAsyncRequest(TargetsToLoad);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	return Streamable.RequestAsyncLoad(TargetsToLoad, MoveTemp(InDelegate));
}

bool UItemDataAsset::IsLoaded() const
{
	// 반드시 별도 구현 필요
	return PickupClass.IsValid();
}

void UItemDataAsset::OnAsyncRequest(TArray<FSoftObjectPath>& InOutArray) const
{
	InOutArray.Add(PickupClass.ToSoftObjectPath());
	
	// 상속받은 클래스에서 추가 요소들 추가 등록
}
