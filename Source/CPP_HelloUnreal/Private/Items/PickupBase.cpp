// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickupBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
	SphereCollision->InitSphereRadius(100.f);
	SetRootComponent(SphereCollision);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetCollisionProfileName("NoCollision");

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	NiagaraComponent->SetupAttachment(SphereCollision);
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
	UE_LOG(LogTemp, Log, TEXT("%s가 %s을 획득했습니다."), *InTarget->GetName(), *GetName());
	bIdle = false;
}

void APickupBase::OnUpdateUpdownSpin(float InDeltaTime)
{
	if (!IsCurveAssetReady()) return;

	ElapsedTime += InDeltaTime;

	float Progress = FMath::Fmod(ElapsedTime / UpDownDuration, 1.f);
	FVector NewMeshLocation = MeshBaseLocation;
	NewMeshLocation.Z += UpDownCurve->GetFloatValue(Progress) * UpDownHeight;

	Mesh->SetRelativeLocation(NewMeshLocation);

	float NewAngle = SpinCurve->GetFloatValue(Progress) * 360.f;
	Mesh->SetRelativeRotation(FRotator(0.f, NewAngle, 0.f));

}

bool APickupBase::IsCurveAssetReady() const
{
	return UpDownCurve && SpinCurve;
}

