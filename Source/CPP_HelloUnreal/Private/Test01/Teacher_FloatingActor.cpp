// Fill out your copyright notice in the Description page of Project Settings.


#include "Test01/Teacher_FloatingActor.h"

// Sets default values
ATeacher_FloatingActor::ATeacher_FloatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	Mesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ATeacher_FloatingActor::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
	{
		HeightOffset = Mesh->GetRelativeLocation().Z;
	}

}

// Called every frame
void ATeacher_FloatingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	UpdateFloatingMesh();
}

void ATeacher_FloatingActor::UpdateFloatingMesh()
{
	float InCos = ElapsedTime * UE_PI * 2 / MoveDuration;	// ElapsedTime * UE_PI * 2: 1초에 한 바퀴
	float CosValue = FMath::Cos(InCos);	// 1 -> 0 -> -1 -> 0 -> 1 -> ...
	CosValue += 1;								// 2 - > 1 -> 0 -> 1 -> 2 -> ...
	CosValue *= 0.5f;							// 1 -> 0.5 -> 0 -> 0.5 -> 1 -> ...
	CosValue = 1 - CosValue;					// 0 -> 0.5 -> 1 -> 0.5 -> 0 -> ...

	Mesh->SetRelativeLocation(FVector(0.f, 0.f, HeightOffset + CosValue * MoveHeight));

	UE_LOG(LogTemp, Log, TEXT("ElapsedTime: %.2f"), ElapsedTime);

}

