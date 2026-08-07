// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/Items/WeaponDataAsset.h"
#include "Engine/AssetManager.h"

void UWeaponDataAsset::OnAsyncRequest(TArray<FSoftObjectPath>& InOutArray) const
{
	Super::OnAsyncRequest(InOutArray);
	InOutArray.Add(Mesh.ToSoftObjectPath());
	InOutArray.Add(TrailVFX.ToSoftObjectPath());
}

bool UWeaponDataAsset::IsLoaded() const
{
	//return Super::IsLoaded() && Mesh.IsValid() && TrailVFX.IsValid();
	return Super::IsLoaded() && Mesh.IsValid();
}
