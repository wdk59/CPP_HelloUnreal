// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/Items/MiscItemDataAsset.h"

void UMiscItemDataAsset::OnAsyncRequest(TArray<FSoftObjectPath>& InOutArray) const
{
	Super::OnAsyncRequest(InOutArray);
	InOutArray.Add(Mesh.ToSoftObjectPath());
}

bool UMiscItemDataAsset::IsLoaded() const
{
	return Super::IsLoaded() && Mesh.IsValid();
}
