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
}

void APickupBase::InitializePickup(UItemDataAsset* InData)
{
	DataAsset = InData;
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	ElapsedTime = 0.f;
	
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

void APickupBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	OnPickup(OtherActor);
}

void APickupBase::OnPickup(AActor* InTarget)
{
	UE_LOG(LogTemp, Log, TEXT("%s가 %s을 획득했습니다."), InTarget ? *InTarget->GetName() : TEXT("알 수 없는 대상"), *this->GetName());
	bIdle = false;

	if (IInventoryUserInterface* Inven = Cast<IInventoryUserInterface>(InTarget))
	{
		FInventoryCommand Command = FInventoryCommand::MakeAdd(DataAsset, 1);
		FInventoryCommandResult Result;
		if (!Inven->ExecuteInventoryCommand(Command, Result))
		{
			UPickupFactorySubsystem* Factory = GetWorld()->GetSubsystem<UPickupFactorySubsystem>();
			Factory->SpawnPickupAsync(DataAsset, InTarget->GetActorTransform(),
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
			OnFinishPickupEffect();
		}
	}
}

void APickupBase::OnUpdatePickupEffect()
{

}
void APickupBase::OnFinishPickupEffect()
{
	Destroy();
}

void APickupBase::OnUpdateUpdownSpin(float InDeltaTime)
{
	if (!IsCurveAssetReady()) return;

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
	return nullptr;
}

bool APickupBase::IsCurveAssetReady() const
{
	return UpDownCurve && SpinCurve;
}
