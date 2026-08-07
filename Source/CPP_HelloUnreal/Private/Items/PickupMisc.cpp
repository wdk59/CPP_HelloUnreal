// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickupMisc.h"
#include "Datas/Items/MiscItemDataAsset.h"
#include "Components/SphereComponent.h"

APickupMisc::APickupMisc()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetCollisionProfileName("NoCollision");
}

void APickupMisc::InitializePickup(UItemDataAsset* InData)
{
	Super::InitializePickup(InData);

	if (DataAsset)
	{
		MiscData = Cast<UMiscItemDataAsset>(DataAsset);
		if (UStaticMesh* MeshData = MiscData->Mesh.LoadSynchronous())
		{
			Mesh->SetStaticMesh(MeshData);
			Mesh->SetRelativeLocation(MeshBaseLocation + MiscData->SpawnLocationOffset);
		}

	}
}

// UPROPERTY 멤버가 바뀌거나 새로 생성되거나 할 때 실행: 블루프린트에서 메시 바뀌는 거 확인 가능
// PostInitializeComponents 이벤트도 나쁘진 않음. 하지만 블프에서 보려면 OnConstruction이 굿. 자세한 건 AActor 주석 참고.
void APickupMisc::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SphereCollision->InitSphereRadius(200.f);

	InitializePickup(DataAsset);

}
