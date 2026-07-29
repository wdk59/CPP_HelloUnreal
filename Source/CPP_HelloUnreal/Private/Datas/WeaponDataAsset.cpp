// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/WeaponDataAsset.h"
#include "Engine/AssetManager.h"

void UWeaponDataAsset::RequestDataLoad(FStreamableDelegate InDelegate)
{
	TArray<FSoftObjectPath> TargetsToLoad;
	TargetsToLoad.Add(Mesh.ToSoftObjectPath());

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	
	AsyncLoadHandle = Streamable.RequestAsyncLoad(TargetsToLoad, MoveTemp(InDelegate));
}

bool UWeaponDataAsset::IsLoadCompleted() const
{
	return false;
}
