// Fill out your copyright notice in the Description page of Project Settings.


#include "Test02/Pickup_Teacher.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Test02/StatComponent.h"
#include "../Interface/StaminaInterface.h"
#include "../Interface/HealthInterface.h"
#include "../Interface/StatInterface.h"

// Sets default values
APickup_Teacher::APickup_Teacher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
	SphereCollision->InitSphereRadius(100.f);
	SetRootComponent(SphereCollision);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphereCollision);

}

// Called when the game starts or when spawned
void APickup_Teacher::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup_Teacher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup_Teacher::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	ApplyEffects(OtherActor);

}

void APickup_Teacher::ApplyEffects(AActor* InTarget)
{
	// bImplements이 true면 인터페이스를 구현했다.
	// bool bImplements = OtherActor->Implements<UStaminaInterface>()

	if (IStatInterface* Stat = Cast<IStatInterface>(InTarget))
	{
		UStatComponent* StatComp = Stat->GetStatComponent();
		if (Stamina > 0)
		{
			IStaminaInterface::Execute_RecoveryStamina(StatComp, Stamina);
		}
		else if (Stamina < 0)
		{
			IStaminaInterface::Execute_ConsumeStamina(StatComp, -Stamina);
		}

		if (Health > 0)
		{
			IHealthInterface::Execute_RecoveryHealth(StatComp, Health);
		}
		else if (Health < 0)
		{
			IHealthInterface::Execute_ReceiveDamage(StatComp, -Health);
		}
	}

	// Target이 null이 아니면 인터페이스를 상속받았다(= C++니까 구현도 되어 있다. 블루프린트에서 상속을 했을 경우는 체크 불가능)
	// IStaminaInterface* Target = Cast<IStaminaInterface>(OtherActor);
}
