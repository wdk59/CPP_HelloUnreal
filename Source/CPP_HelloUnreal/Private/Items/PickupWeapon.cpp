// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickupWeapon.h"
#include "Weapons/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"

#include "Components/SphereComponent.h"

APickupWeapon::APickupWeapon()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetCollisionProfileName("NoCollision");
}

void APickupWeapon::InitializePickup(const UItemDataAsset* InData)
{
	Super::InitializePickup(InData);

	if (DataAsset)
	{
		WeaponData = Cast<const UWeaponDataAsset>(DataAsset);
		if (USkeletalMesh* MeshData = WeaponData->Mesh.LoadSynchronous())
		{
			Mesh->SetSkeletalMesh(MeshData);
			Mesh->SetRelativeLocation(MeshBaseLocation + WeaponData->SpawnLocationOffset);
		}
	}
}
