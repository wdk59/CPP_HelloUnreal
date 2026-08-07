// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UWeaponDataAsset;

UCLASS()
class CPP_HELLOUNREAL_API APickupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBase();

	virtual void InitializePickup(UWeaponDataAsset* InData);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data");
	TObjectPtr<UWeaponDataAsset> DataAsset = nullptr;

	// 메시의 기본 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data");
	FVector MeshBaseLocation = FVector(0.f, 0.f, 50.f);

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

private :

	float ElapsedTime = 0.f;

	bool bIdle = true;

protected:

	// 오버랩 됐을 때 대상에게 효과를 적용하는 함수
	virtual void OnPickup(AActor* InTarget);

	virtual void OnUpdateUpdownSpin(float InDeltaTime);

private :

	bool IsCurveAssetReady() const;

};
