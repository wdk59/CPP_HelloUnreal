// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "Components/StatComponent.h"
#include "Interface/HealthInterface.h"
#include "CommonHeader/ItemDropTable.h"
#include "Items/PickupBase.h"
#include "Items/PickupWeapon.h"
#include "Frameworks/Subsystem/PickupFactorySubsystem.h"

#include "CPP_HelloUnreal/CPP_HelloUnreal.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComp"));

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UStatComponent* AEnemyCharacter::GetStatComponent() const
{
	return StatComp;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(StatComp))
	{
		StatComp->OnDie.AddDynamic(this, &AEnemyCharacter::OnDie);
	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	IHealthInterface::Execute_ReceiveDamage(StatComp, Damage);

	return Damage;
}

void AEnemyCharacter::OnItemDrop()
{
	if (!GetWorld())
		return;

	if (ItemDropTable)
	{
		//TMap<FName, uint8*> Map = ItemDropTable->GetRowMap();
		//for (auto& pair : Map)
		//{
		//	UE_LOG(LogTemp, Log, TEXT("%s 생성 시도"), *pair.Key.ToString());
		//	//FItemDropTableRow* Row = reinterpret_cast<FItemDropTableRow*>(pair.Value);
		//	//(FItemDropTableRow*)(pair.Value);
		//}

		TArray<FItemDropTableRow*> AllRows;
		ItemDropTable->GetAllRows(TEXT("AEnemyCharacter::OnItemDrop"), AllRows);
		for (FItemDropTableRow* Row : AllRows)
		{
			// 필수 데이터 확인
			if (!Row || !Row->PickupData)
				continue;	// 행 정보가 잘못됐으면 그 행(아이템) 스킵
				
			// 드랍 확률 체크
			if (FMath::FRand() > Row->DropRate)
				continue;	// 드랍 확률따라 행(아이템) 스킵

			UPickupFactorySubsystem* PickupFactory = GetWorld()->GetSubsystem<UPickupFactorySubsystem>();
			if (PickupFactory)
			{
				//PickupFactory->SpawnPickup(Row->PickupData, GetActorTransform());
				PickupFactory->SpawnPickupAsync(Row->PickupData, GetActorTransform(),
					FOnPickupSpawned::CreateWeakLambda(
						this,
						[](APickupBase* InSpawned)
						{
							UE_LOG(LogTemp, Log, TEXT("%s가 스폰되었습니다."), *InSpawned->GetName());
						}
					)
				);
			}
		}
	}
}

void AEnemyCharacter::OnDie()
{
	UE_LOG(LogTemp, Log, TEXT("%s가 죽었습니다."), *this->GetName());
	
	// 아이템 드랍 처리
	OnItemDrop();
}

void AEnemyCharacter::SpawnPickup(UItemDataAsset* ItemDataAsset)
{
	//GetWorld()->SpawnActor<AActor>(PickupClass, GetActorTransform());
	if (APickupBase* PickupActor = GetWorld()->SpawnActor<APickupBase>(ItemDataAsset->PickupClass.Get(), GetActorTransform()))
	{
		PickupActor->InitializePickup(ItemDataAsset);
		UE_LOG(LogTemp, Log, TEXT("%s가 드랍되었습니다."), *(ItemDataAsset->DisplayName).ToString());
	}
}
