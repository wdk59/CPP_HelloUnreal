// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickupBase.h"
#include "Components/SphereComponent.h"
#include "Components/MeshComponent.h"
#include "NiagaraComponent.h"
#include "Interface/InventoryUserInterface.h"
#include "Components/InventoryCommandTypes.h"
#include "Frameworks/Subsystem/PickupFactorySubsystem.h"
#include "CPP_HelloUnreal/CPP_HelloUnreal.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
	SphereCollision->InitSphereRadius(100.f);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Player, ECR_Overlap);
	SetRootComponent(SphereCollision);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	NiagaraComponent->SetupAttachment(SphereCollision);
	//NiagaraComponent->Activate();	// 이 나이아가라 컴포넌트가 가지고 있는 나이아가라 에셋을 재생
	//NiagaraComponent->Deactivate();	// 이 나이아가라 컴포넌트가 가지고 있는 나이아가라 에셋을 재생 중지
}

void APickupBase::InitializePickup(const UItemDataAsset* InData)
{
	DataAsset = InData;
}

void APickupBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	InitializePickup(DataAsset);
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	ElapsedTime = 0.f;

	FTimerHandle PickupDelayHandle;
	GetWorld()->GetTimerManager().SetTimer(
		PickupDelayHandle,
		[this]()
		{
			UE_LOG(LogTemp, Log, TEXT("픽업을 획득할 수 있습니다."));
			OnActorBeginOverlap.AddDynamic(this, &APickupBase::OnBeginOverlap);
		},
		PickupDelayTime,
		false
	);
	
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIdle)
	{
		OnUpdateUpdownSpin(DeltaTime);
	}

}

void APickupBase::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	OnPickup(OtherActor);
}

void APickupBase::OnPickup(AActor* InTarget)
{
	if (GetWorldTimerManager().IsTimerActive(PickupEffectTimerHandle))
		return;

	UE_LOG(LogTemp, Log, TEXT("%s가 %s을 획득했습니다."), InTarget ? *InTarget->GetName() : TEXT("알 수 없는 대상"), *this->GetName());
	bIdle = false;

	TargetActor = InTarget;

	// 커브 에셋이 준비되어 있고 메시 컴포넌트가 있으면 연출 시작, 없으면 즉시 획득 처리
	if (IsPickupEffectAssetReady() && GetMesh())
	{
		// 더 이상의 오버랩이 발생하지 않게 하기
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		PickupStartLocation = GetMesh()->GetComponentLocation();
		PickupElapsedTime = 0.f;

		GetWorldTimerManager().SetTimer(
			PickupEffectTimerHandle,
			this,
			&APickupBase::OnUpdatePickupEffect,
			TimerInterval,
			true
		);
	}
	else
	{
		OnFinishPickupEffect();
	}
}

void APickupBase::OnUpdatePickupEffect()
{
	if (!TargetActor.IsValid())
	{
		OnFinishPickupEffect();
		return;
	}

	PickupElapsedTime += TimerInterval;
	float Div = FMath::Max(PickupEffectDuration, 0.001f);
	float Progress = PickupElapsedTime / Div;

	// 플레이어한테 가까이 오기 보간
	float DistanceAlpha = PickupAlphaCurve->GetFloatValue(Progress);
	FVector Goal = TargetActor.Get()->GetActorLocation();
	FVector NewLocation = FMath::Lerp(PickupStartLocation, Goal, DistanceAlpha);

	// 높이 조정
	float HeightOffset = PickupHeightCurve->GetFloatValue(Progress) * PickupEffectHeight;
	NewLocation.Z += HeightOffset;

	GetMesh()->SetWorldLocation(NewLocation);

	float Scale = PickupScaleCurve->GetFloatValue(Progress);
	GetMesh()->SetRelativeScale3D(FVector(Scale));

	if (Progress >= 1.f)
	{
		OnFinishPickupEffect();
	}
}

void APickupBase::OnFinishPickupEffect()
{
	// 연출이 완전히 종료된 후 실제 획득 실행

	// 획득 이펙트용 타이머 클리어
	GetWorldTimerManager().ClearTimer(PickupEffectTimerHandle);

	// 대상의 인벤토리에 아이템 추가
	if (IInventoryUserInterface* Inven = Cast<IInventoryUserInterface>(TargetActor))
	{
		FInventoryCommand Command = FInventoryCommand::MakeAdd(DataAsset, 1);
		FInventoryCommandResult Result;
		if (!Inven->ExecuteInventoryCommand(Command, Result))
		{
			UPickupFactorySubsystem* Factory = GetWorld()->GetSubsystem<UPickupFactorySubsystem>();
			FTransform SpawnTransform = TargetActor->GetActorTransform();
			FVector NewLocation(FMath::RandPointInCircle(300.f), 0.f);	// 액터 위치를 중심으로 반경 3m 서클 안 랜덤 위치
			Factory->SpawnPickupAsync(DataAsset, SpawnTransform,
				FOnPickupSpawned::CreateWeakLambda(
					this,
					[this](APickupBase* InSpawned)
					{
						UE_LOG(LogTemp, Log, TEXT("%s가 스폰되었습니다."), *InSpawned->GetName());
						OnFinishPickupEffect();
					}
				)
			);
		}
		else
		{
			// 성공했으면 인벤토리에 들어갔으니 픽업 삭제
			Destroy();
		}
	}
}

void APickupBase::OnUpdateUpdownSpin(float InDeltaTime)
{
	if (!IsCurveAssetReady())
		return;

	ElapsedTime += InDeltaTime;

	if (UMeshComponent* PickupMesh = GetMesh())
	{
		float Div = FMath::Max(UpDownDuration, 0.001f);
		float Progress = FMath::Fmod(ElapsedTime / Div, 1.f);
		FVector NewMeshLocation = MeshBaseLocation;
		NewMeshLocation.Z += UpDownCurve->GetFloatValue(Progress) * UpDownHeight;

		PickupMesh->SetRelativeLocation(NewMeshLocation);

		float NewAngle = SpinCurve->GetFloatValue(Progress) * 360.f;
		PickupMesh->SetRelativeRotation(FRotator(0.f, NewAngle, 0.f));

	}

}

UMeshComponent* APickupBase::GetMesh() const
{
	return nullptr;	// 아직 Mesh가 없기 때문에 반환X. Mesh는 상속받는 액터에서 결정.
}

bool APickupBase::IsCurveAssetReady() const
{
	return UpDownCurve && SpinCurve;
}

bool APickupBase::IsPickupEffectAssetReady() const
{
	return PickupAlphaCurve && PickupHeightCurve && PickupScaleCurve;
}
