// Fill out your copyright notice in the Description page of Project Settings.


#include "0720/Practice/FloatingActor.h"

// Sets default values
AFloatingActor::AFloatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

}

// Called when the game starts or when spawned
void AFloatingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloatingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FloatingByCos(DeltaTime);

	ElapsedTime += DeltaTime;
}

void AFloatingActor::FloatingByCos()
{
	CurrentCos = FMath::Cos(ElapsedTime);

	Mesh->AddRelativeLocation(FVector(0.f, 0.f, 1.f) * (PreCos - CurrentCos) * MoveSpeed);

	PreCos = CurrentCos;
}

