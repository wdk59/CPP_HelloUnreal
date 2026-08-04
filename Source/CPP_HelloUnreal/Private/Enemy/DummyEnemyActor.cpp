// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DummyEnemyActor.h"
#include "Config/ObjectPoolSettings.h"
#include "Frameworks/Subsystem/ObjectPoolSubsystem.h"
#include "Enemy/DamagePopupActor.h"
#include "Enemy/BloodPopupActor.h"

// Sets default values
ADummyEnemyActor::ADummyEnemyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	PopupTransform = CreateDefaultSubobject<USceneComponent>(TEXT("PopupTransform"));
	PopupTransform->SetupAttachment(Mesh);
	PopupTransform->SetRelativeLocation(FVector(0.f, 0.f, 70.f));

	BloodTransform = CreateDefaultSubobject<USceneComponent>(TEXT("BloodTransform"));
	BloodTransform->SetupAttachment(Mesh);
	BloodTransform->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	// 프로젝트 세팅에서 데이터 읽어오기
	const UObjectPoolSettings* Settings = GetDefault<UObjectPoolSettings>();
	if (Settings)
	{
		if (!Settings->DamagePopupClass.IsNull())
		{
			// LoadSynchronous: 즉시 로딩
			DamagePopupType = Settings->DamagePopupClass.LoadSynchronous();
			BloodPopupType = Settings->BloodPopupClass.LoadSynchronous();
		}
	}

}

// Called when the game starts or when spawned
void ADummyEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADummyEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ADummyEnemyActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UObjectPoolSubsystem* SubSystem = GameInstance->GetSubsystem<UObjectPoolSubsystem>();
		
		AActor* SpawnedActor = SubSystem->Spawn(DamagePopupType, PopupTransform->GetComponentTransform());
		ADamagePopupActor* DamagePopup = Cast<ADamagePopupActor>(SpawnedActor);
		DamagePopup->OnPopupStart(Damage);

		AActor* SpawnedBloodActor = SubSystem->Spawn(BloodPopupType, BloodTransform->GetComponentTransform());
		ABloodPopupActor* BloodPopup = Cast<ABloodPopupActor>(SpawnedBloodActor);
	}

	return Damage;
}

