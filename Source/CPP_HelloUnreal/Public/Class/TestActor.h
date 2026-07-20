// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// generated.h는 모든 헤더 중 마지막에 있어야 한다.
#include "TestActor.generated.h"

// UCLASS()
// - C++ 클래스를 언리얼 엔진의 객체 시스템에 등록(= UObject)
// - 가비지 콜렉팅 지원, 블루프린트 연동, 에디터서 노출 등이 가능해짐
// - 다중 상속을 막는다.

// CDO (Class Default Object)
// - 기본 클래스 인스턴스
// - 새 클래스가 인스턴싱될 때 CDO를 클로닝해서 사용
// - 기본값
// - 나중에 따로 한 번 찾아보시면 좋아요~

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

	// Called every frame
	virtual void Tick(float DeltaTime) override;	// Tick을 protected로 변경

protected :
	int32 Data1 = 10;	// C++ 전용 변수. 블루프린트에서 보이지 않고, 접근도 안 됨. C++에서 내부적으로 사용하는 용도. 포인터인 경우에는 가비지 콜렉팅도 안 됨.
	
	UPROPERTY(VisibleAnywhere)	// 클래스 디폴트 창과, 맵에 배치된 액터의 디테일 창에서 확인 가능
	int32 Data2_1 = 21;

	UPROPERTY(VisibleDefaultsOnly)	// 클래스 디폴트 창에서만 보임
	int32 Data2_2 = 22;
	
	UPROPERTY(VisibleInstanceOnly)	// 맵에 배치된 액터의 디테일 창에서만 보임
	int32 Data2_3 = 23;

};
