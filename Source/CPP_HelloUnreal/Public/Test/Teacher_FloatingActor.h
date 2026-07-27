// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Teacher_FloatingActor.generated.h"

UCLASS()
class CPP_HELLOUNREAL_API ATeacher_FloatingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeacher_FloatingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

private :

	// 누적 시간
	float ElapsedTime = 0.0f;

	// 메시의 높이 오프셋
	float HeightOffset = 0.0f;

protected:	
	// 위아래 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveDuration = 5.0f;

	// 위아래 움직이는 높이
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveHeight = 300.0f;

protected :

	// 메시의 움직임을 결정할 함수
	virtual void UpdateFloatingMesh();
	
	inline float GetElapsedTime() const { return ElapsedTime; }
	// 높이 오프셋 확인 함수
	inline float GetHeightOffset() const { return HeightOffset; }

};
