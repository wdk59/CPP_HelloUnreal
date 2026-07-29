// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickupWeapon.h"
#include "Weapons/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"

// UPROPERTY 멤버가 바뀌거나 새로 생성되거나 할 때 실행: 블루프린트에서 메시 바뀌는 거 확인 가능
// PostInitializeComponents 이벤트도 나쁘진 않음. 하지만 블프에서 보려면 OnConstruction이 굿. 자세한 건 AActor 주석 참고.
void APickupWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (WeaponData)
	{
		if (UStaticMesh* StaticMeshData = WeaponData->Mesh.LoadSynchronous())
		{
			Mesh->SetStaticMesh(StaticMeshData);
		}

	}

}

void APickupWeapon::OnPickup(AActor* InTarget)
{
	Super::OnPickup(InTarget);

	
	IWeaponUserInterface::Execute_EquipWeapon(InTarget, WeaponData);

	Destroy();
}
