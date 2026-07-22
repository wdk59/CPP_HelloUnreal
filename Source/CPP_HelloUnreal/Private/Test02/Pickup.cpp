// Fill out your copyright notice in the Description page of Project Settings.


#include "Test02/Pickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Interface/StaminaInterface.h"

// Sets default values
APickup::APickup()
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
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Target이 null이 아니면 인터페이스를 상속받았다.(= C++이니까 구현도 되어 있다. 블루프린트에서 상속을 했을 경우에는 체크 불가능)
	//IStaminaInterface* Target = Cast<IStaminaInterface>(OtherActor);
	// bImplements이 true면 인터페이스를 구현했다.(블루프린트에서의 상속도 체크 가능)
	//bool bImplements = OtherActor->Implements<UStaminaInterface>();

	if (OtherActor && OtherActor->Implements<UStaminaInterface>())
	{
		if (Stamina > 0)
		{
			IStaminaInterface::Execute_RecoveryStamina(OtherActor, Stamina);
		}
		else
		{
			IStaminaInterface::Execute_ConsumeStamina(OtherActor, -Stamina);
		}
	}

}

