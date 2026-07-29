// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponActor.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "CPP_HelloUnreal/CPP_HelloUnreal.h"
#include "Interface/WeaponUserInterface.h"
#include "Kismet/GameplayStatics.h"	// Kismet: 언리얼3 때 쓰던 건데 아직 유지 중
#include "Datas/WeaponDataAsset.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
	SetRootComponent(Mesh);
	//Mesh->SetCollisionProfileName(TEXT("NoCollision"));	// 프로파일을 이용해 한 번에 세팅 (실제 적용되는 타이밍이 늦음)
	
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);


	HitArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitArea"));
	HitArea->SetupAttachment(Mesh);
	HitArea->SetCapsuleHalfHeight(60.f, false);
	HitArea->SetCapsuleRadius(15.f, false);
	HitArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitArea->SetCollisionObjectType(ECC_Weapon);
	HitArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitArea->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
	HitArea->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	HitArea->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnHitAreaBeginOverlap);
	
}

void AWeaponActor::AttackEnable(bool bEnable)
{
	if (bEnable)
	{
		HitArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeaponActor::InitializeWeapon(UWeaponDataAsset* InData)
{
	WeaponData = InData;

	Mesh->SetStaticMesh(WeaponData->Mesh.Get());	// 전제: 실행 시점에 WeaponData의 로딩 완료. Weapon 스폰 시 SpawnActorDeferred로 보장.

	HitArea->SetCapsuleHalfHeight(WeaponData->HitAreaHeight, false);
	HitArea->SetCapsuleRadius(WeaponData->HitAreaRadius, false);
	HitArea->SetRelativeLocation(WeaponData->LocationOffset);

}

void AWeaponActor::EquipToTarget(AActor* Target)
{
	OnEquipped(Target);
}

void AWeaponActor::DropWeapon()
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);	// 현재 월드 상태를 유지하고, 부모 갱신한다는 규칙
	DetachFromActor(DetachRules);	// 규칙 적용해서 현재 부모에게서 떼어냄
	
	Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetSimulatePhysics(true);
	HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// 물리 작용 중에 HitArea로 인한 충돌 방지

}

void AWeaponActor::OnEquipped(AActor* InOwner)
{
	UE_LOG(LogTemp, Log, TEXT("장착한다이"));
	SetOwner(InOwner);
	OwnerCharacter = Cast<ACharacter>(InOwner);
	FAttachmentTransformRules AttachRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		true
	);

	if (OwnerCharacter.IsValid())
	{
		AttachToComponent(OwnerCharacter->GetMesh(), AttachRules, WeaponData->AttachSocketName);
		HitArea->IgnoreActorWhenMoving(OwnerCharacter.Get(), true);	// 이미 OwnerCharacter와의 충돌은 무시되지만, 만약을 대비한 것

		IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter);
		if (WeaponUser)
		{
			WeaponUser->GetWeaponAttackStateChangedDelegate().BindUFunction(this, FName("AttackEnable"));
		}
	}

}

void AWeaponActor::OnHitAreaBeginOverlap(UPrimitiveComponent* InOverlapComponent, AActor* InOtherActor, UPrimitiveComponent* InOthercomp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("가해자: %s"), *OwnerCharacter->GetName());
	UE_LOG(LogTemp, Log, TEXT("피해자: %s"), *InOtherActor->GetName());

	if (OwnerCharacter.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("퍽"));

		UGameplayStatics::ApplyDamage(InOtherActor, WeaponData->AttackPower, OwnerCharacter->GetController(), this, nullptr);	// 호출하면 대상의 TakeDamage 함수가 호출된다.
	}
}
