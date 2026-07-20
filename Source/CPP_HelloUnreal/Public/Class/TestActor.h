// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// generated.h는 모든 헤더 중 마지막에 있어야 한다.
#include "TestActor.generated.h"

// UCLASS()
// - C++ 클래스를 언리얼 엔진의 객체 시스템에 등록(= UObject)
// - 가비지 콜렉팅 지원, 블루프린트

UCLASS()
class CPP_HELLOUNREAL_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
