// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UItemDataAsset;

UCLASS()
class CPP_HELLOUNREAL_API APickupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBase();

	virtual void InitializePickup(const UItemDataAsset* InData);

protected:


	// UPROPERTY 멤버가 바뀌거나 새로 생성되거나 할 때 실행: 블루프린트에서 메시 바뀌는 거 확인 가능
	// PostInitializeComponents 이벤트도 나쁘진 않음. 하지만 블프에서 보려면 OnConstruction이 굿. 자세한 건 AActor 주석 참고.
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> NiagaraComponent = nullptr;

protected:

	// 메시의 기본 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data");
	FVector MeshBaseLocation = FVector(0.f, 0.f, 50.f);

	// 픽업시 획득할 데이터 에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data");
	TObjectPtr<const UItemDataAsset> DataAsset = nullptr;

	// 스폰 직후에 아이템이 안 먹어지는 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data")
	float PickupDelayTime = 1.f;

	// 맵에 있을 때 위아래로 왕복하는 모습용 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Default")
	TObjectPtr<UCurveFloat> UpDownCurve;

	// 맵에 있을 때 회전하는 모습용 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Default")
	TObjectPtr<UCurveFloat> SpinCurve;

	// 위아래로 왕복하는데 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Default")
	float UpDownDuration = 2.f;

	// 위아래로 움직이는 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Default")
	float UpDownHeight = 100.f;

protected :

	// 아이템을 줍는 연출의 진행 상황용 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
	TObjectPtr<UCurveFloat> PickupAlphaCurve;

	// 아이템을 줍는 연출 중 위아래로 움직임을 위한 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
	TObjectPtr<UCurveFloat> PickupHeightCurve;

	// 아이템을 줍는 연출 중 크기 변경을 위한 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
	TObjectPtr<UCurveFloat> PickupScaleCurve;

	// 아이템을 줍는 연출의 전체 진행 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
	float PickupEffectDuration = 0.5f;

	// PickupHeight로 인해 올라가는 높이
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
	float PickupEffectHeight = 50.f;

private :

	float ElapsedTime = 0.f;

	bool bIdle = true;

	// 아이템을 줍는 연출용 타이머 핸들
	FTimerHandle PickupEffectTimerHandle;

	// 아이템을 줍는 대상
	TWeakObjectPtr<AActor> TargetActor = nullptr;

	// 아이템을 줍는 연출이 진행된 시간
	float PickupElapsedTime = 0.f;

	// 아이템을 줍는 연출용 타이머의 실행 간격
	const float TimerInterval = 0.02f;

	// 아이템을 줍는 연출용 시작 위치
	FVector PickupStartLocation;

protected:

	// 오버랩 델리게이트에 바인딩할 함수
	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// 오버랩 됐을 때 대상에게 효과를 적용하는 함수
	virtual void OnPickup(AActor* InTarget);

	virtual void OnUpdatePickupEffect();
	virtual void OnFinishPickupEffect();

	virtual void OnUpdateUpdownSpin(float InDeltaTime);

	virtual UMeshComponent* GetMesh() const;

private :

	bool IsCurveAssetReady() const;
	bool IsPickupEffectAssetReady() const;

};
