// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BloodPopupActor.h"
#include "Frameworks/Subsystem/ObjectPoolSubsystem.h"

#include "NiagaraComponent.h"

// Sets default values
ABloodPopupActor::ABloodPopupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BloodVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BloodVFX"));
	SetRootComponent(BloodVFXComponent);

}

void ABloodPopupActor::UseFinish()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UObjectPoolSubsystem* Subsystem = GameInstance->GetSubsystem<UObjectPoolSubsystem>();
		Subsystem->ReturnPool(this);
	}
}

void ABloodPopupActor::OnSpawn_Implementation()
{
	SetActorHiddenInGame(false);
	//SetActorTickEnabled(false);
	SetActorEnableCollision(false);

	BloodVFXComponent->Activate();
}

void ABloodPopupActor::OnReturn_Implementation()
{
	//BloodVFXComponent->Deactivate();

	SetActorLocation(FVector(0.f, 0.f, -10000.f));
	SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void ABloodPopupActor::BeginPlay()
{
	Super::BeginPlay();

	BloodVFXComponent->OnSystemFinished.AddDynamic(this, &ABloodPopupActor::OnBloodVFXFinished);
	
}

// Called every frame
void ABloodPopupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABloodPopupActor::OnBloodVFXFinished(UNiagaraComponent* FinishedComponent)
{
	UseFinish();
}
