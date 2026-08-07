// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"
#include "Datas/Items/WeaponDataAsset.h"

#include "CPP_HelloUnreal/CPP_HelloUnreal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"	// Kismet: 언리얼3 때 쓰던 건데 아직 유지 중
#include "NiagaraComponent.h"

#include "Components/WeaponComponent.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RootMesh"));
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
	HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitArea->SetCollisionObjectType(ECC_Weapon);
	HitArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitArea->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
	HitArea->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

	TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TraiVFX"));
	TrailVFX->SetupAttachment(Mesh);

}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	HitArea->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnHitAreaBeginOverlap);

	TrailVFX->Deactivate();

}

void AWeaponActor::AttackEnable(bool bEnable)
{
	if (bEnable)
	{
		HitArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		TrailVFX->Activate();
	}
	else
	{
		HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		TrailVFX->Deactivate();
	}
}

void AWeaponActor::InitializeWeapon(UWeaponDataAsset* InData)
{
	if (!InData)	return;

	WeaponData = InData;

	/* 에셋 설정 */

	if (WeaponData->IsLoaded())	// 로딩이 완료되었을 때만 처리
	{
		// 메시 설정 및 위치 조정
		Mesh->SetSkeletalMesh(WeaponData->Mesh.Get());	// 전제: 실행 시점에 WeaponData의 로딩 완료. Weapon 스폰 시 SpawnActorDeferred로 보장.
		//Mesh->SetRelativeLocation(WeaponData->LocationOffset);

		// 나이아가라 설정
		TrailVFX->SetAsset(WeaponData->TrailVFX.Get());

		//HitArea 크기 조정
		HitArea->SetCapsuleHalfHeight(WeaponData->HitAreaHalfHeight, false);
		HitArea->SetCapsuleRadius(WeaponData->HitAreaRadius, false);

		// 사용 횟수 설정
		CurrentUseCount = WeaponData->UseCount;
		UE_LOG(LogTemp, Log, TEXT("Current Use Count: %d"), CurrentUseCount);
	}
}

void AWeaponActor::EquipToTarget(AActor* Target)
{
	OnEquipped(Target);
}

void AWeaponActor::DropWeapon()
{
	if (IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter))
	{
		if (UWeaponComponent* WeaponComp = WeaponUser->GetWeaponComponent())
		{
			WeaponComp->OnWeaponAttackStateChanged.Clear();
		}
	}

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);	// 현재 월드 상태를 유지하고, 부모 갱신한다는 규칙
	DetachFromActor(DetachRules);	// 규칙 적용해서 현재 부모에게서 떼어냄

	//Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Player, ECollisionResponse::ECR_Ignore);
	Mesh->SetSimulatePhysics(true);
	HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// 물리 작용 중에 HitArea로 인한 충돌 방지

	// 일정 시간동안 무기와 플레이어가 충돌 안 하게 설정
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(
		PhysicsDelayTimerHandle,
		FTimerDelegate::CreateWeakLambda(
			this,
			[this]()
			{
				Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Player, ECollisionResponse::ECR_Block);
			}
		),
		PhysicsDelay,
		false
	);

	// 뒤로 던지기
	FVector BackwardDirection = FVector::BackwardVector;
	if (OwnerCharacter.IsValid())
	{
		BackwardDirection = -OwnerCharacter->GetActorForwardVector();
	}

	FVector ThrowDirection = BackwardDirection * ThrowPower + FVector::UpVector * 200.f;

	Mesh->AddImpulse(ThrowDirection, NAME_None, true);

	FVector AngularImpulse = FVector(
		FMath::RandRange(-200.f, 200.f)
	) + GetActorForwardVector() * ThrowPower;
	Mesh->AddAngularImpulseInDegrees(AngularImpulse, NAME_None, true);

	// DropLifeSpan초 후에 무기 액터 제거
	SetLifeSpan(DropLifeSpan);

	OnWeaponDrop.Unbind();
	OwnerCharacter = nullptr;

}

void AWeaponActor::Use()
{
	if (WeaponData && !WeaponData->bInfinityUse)
	{
		CurrentUseCount--;
		UE_LOG(LogTemp, Log, TEXT("Current Use Count: %d"), CurrentUseCount);

		if (CurrentUseCount <= 0)
		{
			OnWeaponDrop.ExecuteIfBound(WeaponData);
		}
	}
}

void AWeaponActor::ResetUseCount()
{
	CurrentUseCount = WeaponData->UseCount;
	UE_LOG(LogTemp, Log, TEXT("Current Use Count: %d"), CurrentUseCount);
}

FVector AWeaponActor::GetWeaponImpactLocation() const
{
	return FMath::Lerp(Mesh->GetSocketLocation(TEXT("Tip")), Mesh->GetSocketLocation(TEXT("Base")), 0.5f);
}

void AWeaponActor::OnEquipped(AActor* InOwner)
{
	if (!WeaponData)
	{
		return;
	}

	//UE_LOG(LogTemp, Log, TEXT("장착한다이"));
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
	
		// 오프셋 적용
		SetActorRelativeLocation(WeaponData->AttachLocationOffset);
		
		HitArea->IgnoreActorWhenMoving(OwnerCharacter.Get(), true);	// 이미 OwnerCharacter와의 충돌은 무시되지만, 만약을 대비한 것

		IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter);
		if (WeaponUser)
		{
			if (UWeaponComponent* WeaponComp = WeaponUser->GetWeaponComponent())
			{
				WeaponComp->OnWeaponAttackStateChanged.BindUFunction(this, FName("AttackEnable"));
			}
		}
	}

}

void AWeaponActor::OnHitAreaBeginOverlap(UPrimitiveComponent* InOverlapComponent, AActor* InOtherActor, UPrimitiveComponent* InOthercomp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
	if (!OwnerCharacter.IsValid() || !InOtherActor)
		return;

	float Damage = WeaponData ? WeaponData->AttackPower : 1;

	UE_LOG(LogTemp, Log, TEXT("가해자: %s"), *OwnerCharacter->GetName());
	UE_LOG(LogTemp, Log, TEXT("피해자: %s"), *InOtherActor->GetName());

	UGameplayStatics::ApplyDamage(InOtherActor, Damage, OwnerCharacter->GetController(), this, nullptr);	// 호출하면 대상의 TakeDamage 함수가 호출된다.
}
